#include "GameRenderer.h"
#include "AppConfig.h"
#include "AssetDefs.h"
#include "Board.h"
#include "GameDrawOrder.h"

#include <engine/Engine.h>
#include <engine/GlProgram.h>
#include <engine/Graphics.h>

using namespace Wind;

class GameRenderer::Impl {
public:
	explicit Impl(Graphics& graphics)
	    : mGraphics { graphics } {

		PipelineState pipelineState;
		pipelineState.mBlending = true;
		pipelineState.mSrcAlpha = GL_SRC_ALPHA;
		pipelineState.mDstAlpha = GL_ONE_MINUS_SRC_ALPHA;
		mPipelineBlending = graphics.NewPipeline(pipelineState);

		pipelineState.mBlending = true;
		pipelineState.mSrcAlpha = GL_SRC_ALPHA;
		pipelineState.mDstAlpha = GL_ONE;
		mPipelineAdditive = graphics.NewPipeline(pipelineState);

		mTileProgram.mProgramHandle = graphics.NewProgram(SHADERS_FOLDER "tile.vs", SHADERS_FOLDER "tile.fs");
		if (mTileProgram.mProgramHandle != nullProgram) {
			const GlProgram& program = graphics.GetProgram(mTileProgram.mProgramHandle);
			mTileProgram.mCoords = program.GetAttribLocation("tileCoords");
			mTileProgram.mTexture = program.GetUniformLocation("inputTexture");
			mTileProgram.mValid = (mTileProgram.mCoords != -1 && mTileProgram.mTexture != -1);
		}

		mIconProgram.mProgramHandle = graphics.NewProgram(SHADERS_FOLDER "piece.vs", SHADERS_FOLDER "piece.fs");
		if (mIconProgram.mProgramHandle != nullProgram) {
			const GlProgram& program = graphics.GetProgram(mIconProgram.mProgramHandle);
			mIconProgram.mTransform = program.GetUniformLocation("transform");
			mIconProgram.mTileSize = program.GetUniformLocation("tileSize");
			mIconProgram.mColor = program.GetUniformLocation("pieceColor");
			mIconProgram.mTexture = program.GetUniformLocation("colorTexture");
			mIconProgram.mValid = (mIconProgram.mTransform != -1 && mIconProgram.mTileSize != -1 && mIconProgram.mTexture != -1);
		}

		mTrailProgram.mProgramHandle = graphics.NewProgram(SHADERS_FOLDER "trail.vs", SHADERS_FOLDER "trail.fs");
		if (mTrailProgram.mProgramHandle != nullProgram) {
			const GlProgram& program = graphics.GetProgram(mTrailProgram.mProgramHandle);
			mTrailProgram.mCoords = program.GetUniformLocation("coords");
			mTrailProgram.mWidth = program.GetUniformLocation("width");
			mTrailProgram.mColor = program.GetUniformLocation("color");
			mTrailProgram.mTexture = program.GetUniformLocation("inputTexture");
			mTrailProgram.mValid = (mTrailProgram.mCoords != -1 && mTrailProgram.mTexture != -1);
		}

		mBlastProgram.mProgramHandle = graphics.NewProgram(SHADERS_FOLDER "blast.vs", SHADERS_FOLDER "blast.fs");
		if (mBlastProgram.mProgramHandle != nullProgram) {
			const GlProgram& program = graphics.GetProgram(mBlastProgram.mProgramHandle);
			mBlastProgram.mCoords = program.GetUniformLocation("coords");
			mBlastProgram.mColor = program.GetUniformLocation("color");
			mBlastProgram.mTexture = program.GetUniformLocation("inputTexture");
			mBlastProgram.mValid = (mBlastProgram.mCoords != -1 && mBlastProgram.mTexture != -1);
		}

		for (int i = 0; i < NumGameTextures; ++i) {
			gameTextures[i] = graphics.LoadTexture(gameTexturePath[i]);
		}
	}

	void DrawBackgroundTiles(const Board& board, const AppConfig& gameConfig) const {
		if (! mTileProgram.mValid) {
			return;
		}
		const float cellWidth = gameConfig.board.cellWidth;
		const float cellHeight = gameConfig.board.cellHeight;
		const float cellSpacing = gameConfig.board.cellSpacing;
		mGraphics.SetPipeline(mPipelineBlending);

		const GLuint   textureID = gameTextures[boardTileIcons[0]]->GetTextureId();
		const unsigned textureIds[] = { textureID };

		struct Tile {
			Vec4 coords;
			Vec4 color;
		};
		InstanceData instanceData = mGraphics.AllocInstances((unsigned)board.GetCells().Size(), sizeof(Tile), mTileProgram.mCoords);
		if (! instanceData.data) {
			return;
		}

		int   idx = 0;
		Tile* tiles = static_cast<Tile*>(instanceData.data);
		for (const Cell& cell : board.GetCells()) {
			if (cell.category != CellCategory::hole) {
				auto visual = static_cast<const CellVisual*>(cell.ud);
				tiles[idx].coords = { cell.coords.x - cellSpacing, cell.coords.y - cellSpacing, cellWidth + 2 * cellSpacing,
					                  cellHeight + 2 * cellSpacing };
				tiles[idx].color = { 1.f, 1.f, 1.f, 140.f / 255.f * visual->bkgAlpha };
				++idx;
			}
		}
		if (idx > 0) {
			DrawCall drawCall;
			drawCall.program = mTileProgram.mProgramHandle;
			drawCall.mesh = quadMesh;
			drawCall.drawOrder = GameDrawOrder::backgroundTile;
			drawCall.sortKey = (textureID & 255); // sort by texture
			drawCall.textures = textureIds;
			drawCall.numTextures = 1;
			drawCall.instances = instanceData;
			mGraphics.Draw(drawCall);
		}
	}

	void DrawPieces(const Board& board, const AppConfig& gameConfig, float time) const {
		if (! mIconProgram.mValid) {
			return;
		}
		const float dynScale = 1.15f + 0.15f * std::sin(time * 16.f);
		const float dynOffset = 5.f * std::sin(time * 32.f);
		const float dynColor = 1.f + (0.5f + 0.5f * std::sin(time * 16.f));
		const float cellWidth = gameConfig.board.cellWidth;
		const float cellHeight = gameConfig.board.cellHeight;
		mGraphics.SetPipeline(mPipelineBlending);

		const int uniforms[] = {
			mIconProgram.mTransform,
			mIconProgram.mTileSize,
			mIconProgram.mColor,
		};

		for (const Cell& cell : board.GetCells()) {
			auto visual = static_cast<const CellVisual*>(cell.ud);
			if (visual->bitmapIdx < 0) {
				continue;
			}
			Vec2  offset { 0.f, 0.f };
			Vec4  color { 1.f, 1.f, 1.f, 1.f };
			float s = visual->scale;
			if (cell.hasEffect) {
				color.x = dynColor;
				color.y = dynColor;
				color.z = dynColor;
				switch (cell.effectType) {
				case EffectType::hrocket:
					offset.x = dynOffset;
					break;
				case EffectType::vrocket:
					offset.y = dynOffset;
					break;
				default:
					s *= dynScale;
					break;
				}
			}

			const float uniformData[] = {
				visual->coords.x + cellWidth * 0.5f + offset.x,
				visual->coords.y + cellHeight * 0.5f + offset.y,
				std::cos(visual->rotation),
				std::sin(visual->rotation),
				cellWidth * s,
				cellHeight * s,
				0.f,
				0.f,
				color.x,
				color.y,
				color.z,
				color.w,
			};

			//  const GLuint hrzStripesId = gameTextures[hrzStripesSprite]->GetTextureId();
			const unsigned textureIds[] = { gameTextures[visual->bitmapIdx]->GetTextureId(), 0 };

			DrawCall drawCall;
			drawCall.program = mIconProgram.mProgramHandle;
			drawCall.mesh = quadMesh;
			drawCall.drawOrder = GameDrawOrder::boardPiece;
			drawCall.sortKey = textureIds[0]; // sort by main texture
			drawCall.textures = textureIds;
			drawCall.numTextures = 1;
			drawCall.uniformLocations = uniforms;
			drawCall.uniforms = uniformData;
			drawCall.numUniforms = std::size(uniforms);
			mGraphics.Draw(drawCall);

			if (cell.layers > 0) {
				DrawIcon(cell.coords + Vec2 { cellWidth, cellHeight } * 0.5f, iceSprites[0], 0.f, whiteColor,
				         static_cast<unsigned>(GameDrawOrder::ice));
			}
		}
	}

	void DrawTrail(Vec2 start, Vec2 end, float w, const Color& color) const {
		if (! mTrailProgram.mValid) {
			return;
		}

		mGraphics.SetPipeline(mPipelineAdditive);

		const Texture& texture = *gameTextures[glowSprite];
		const int      uniforms[] = { mTrailProgram.mCoords, mTrailProgram.mWidth, mTrailProgram.mColor };
		unsigned       textureIds[] = { texture.GetTextureId() };
		const float    uniformData[] = {
            start.x, start.y, end.x, end.y, w, 0.f, 0.f, 0.f, color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f,
		};
		const DrawCall drawCall {
			.uniformLocations = uniforms,
			.uniforms = uniformData,
			.numUniforms = std::size(uniforms),
			.textures = textureIds,
			.numTextures = 1,
			.program = mTrailProgram.mProgramHandle,
			.mesh = quadMesh,
			.drawOrder = GameDrawOrder::overlays,
		};
		mGraphics.Draw(drawCall);
	}

	void DrawBlast(Vec2 center, float radius, float width, const Color& color) const {
		if (! mBlastProgram.mValid) {
			return;
		}

		mGraphics.SetPipeline(mPipelineAdditive);

		const Texture& texture = *gameTextures[blastSprite];
		const int      uniforms[] = { mBlastProgram.mCoords, mBlastProgram.mColor };
		unsigned       textureIds[] = { texture.GetTextureId() };
		const float    uniformData[] = {
            center.x,        center.y,        radius,          width, //
            color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f,
		};
		const DrawCall drawCall {
			.uniformLocations = uniforms,
			.uniforms = uniformData,
			.numUniforms = std::size(uniforms),
			.textures = textureIds,
			.numTextures = 1,
			.program = mBlastProgram.mProgramHandle,
			.mesh = quadMesh,
			.drawOrder = GameDrawOrder::overlays,
		};
		mGraphics.Draw(drawCall);
	}

	void DrawIcon(Vec2 coords, uint32_t iconIdx, float rotation, const Color& color, unsigned drawOrder) const {
		if (! mIconProgram.mValid) {
			return;
		}
		mGraphics.SetPipeline(mPipelineBlending);

		const int uniforms[] = {
			mIconProgram.mTransform,
			mIconProgram.mTileSize,
			mIconProgram.mColor,
		};

		const TexturePtr& texture = gameTextures[iconIdx];

		const float    uniformData[] = { coords.x,
			                             coords.y,
			                             std::cos(rotation),
			                             std::sin(rotation), //
			                             (float)texture->Width(),
			                             (float)texture->Height(),
			                             0.f,
			                             0.f,
			                             color.r / 255.f,
			                             color.g / 255.f,
			                             color.b / 255.f,
			                             color.a / 255.f };
		const unsigned textureIds[] = { texture->GetTextureId(), 0 };
		const DrawCall drawCall {
			.uniformLocations = uniforms,
			.uniforms = uniformData,
			.numUniforms = std::size(uniforms),
			.textures = textureIds,
			.numTextures = 1,
			.program = mIconProgram.mProgramHandle,
			.mesh = quadMesh,
			.drawOrder = drawOrder,
			.sortKey = textureIds[0], // sort by main texture
		};
		mGraphics.Draw(drawCall);
	}

private:
	struct TileProgram {
		ProgramHandle mProgramHandle = nullProgram;
		GLint         mCoords = 0;
		GLint         mTexture = 0;
		bool          mValid = false;
	};

	struct PieceProgram {
		ProgramHandle mProgramHandle = nullProgram;
		GLint         mTransform = 0;
		GLint         mColor = 0;
		GLint         mMisc = 0;
		GLint         mTileSize = 0;
		GLint         mTexture = 0;
		GLint         mMaskTexture = 0;
		bool          mValid = false;
	};

	struct TrailProgram {
		ProgramHandle mProgramHandle = nullProgram;
		GLint         mCoords = 0;
		GLint         mWidth = 0;
		GLint         mTexture = 0;
		GLint         mColor = 0;
		bool          mValid = false;
	};

	struct BlastProgram {
		ProgramHandle mProgramHandle = nullProgram;
		GLint         mCoords = 0;
		GLint         mTexture = 0;
		GLint         mColor = 0;
		bool          mValid = false;
	};

	Graphics&      mGraphics;
	PipelineHandle mPipelineBlending;
	PipelineHandle mPipelineAdditive;
	TileProgram    mTileProgram;
	PieceProgram   mIconProgram;
	TrailProgram   mTrailProgram;
	BlastProgram   mBlastProgram;
};

GameRenderer::GameRenderer(Engine& engine)
    : mPimpl { std::make_unique<Impl>(engine.GetGraphics()) } {
}

GameRenderer::~GameRenderer() = default;

void GameRenderer::DrawBoard(const Board& board, int selectedCell, const AppConfig& gameConfig, float time) const {
	mPimpl->DrawBackgroundTiles(board, gameConfig);
	mPimpl->DrawPieces(board, gameConfig, time);
	if (selectedCell >= 0) {
		const Cell& cell = board.GetCell(selectedCell);
		mPimpl->DrawIcon(cell.coords + Vec2 { TileWidth, TileHeight } * 0.5f, selectionSprite, 0.f, whiteColor,
		                 static_cast<unsigned>(GameDrawOrder::overlays));
	}
}

void GameRenderer::DrawLaser(Vec2 start, Vec2 end, float w, const Color& color) const {
	mPimpl->DrawTrail(start, end, w, color);
}

void GameRenderer::DrawBlast(Vec2 center, float radius, float width, const Color& color) const {
	mPimpl->DrawBlast(center, radius, width, color);
}

void GameRenderer::DrawIcon(uint32_t iconIdx, Vec2 coords, float rotation, const Color& color, unsigned drawOrder) const {
	mPimpl->DrawIcon(coords, iconIdx, rotation, color, drawOrder);
}