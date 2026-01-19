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

		mPieceProgram.mProgramHandle = graphics.NewProgram(SHADERS_FOLDER "piece.vs", SHADERS_FOLDER "piece.fs");
		if (mPieceProgram.mProgramHandle != nullProgram) {
			const GlProgram& program = graphics.GetProgram(mPieceProgram.mProgramHandle);
			mPieceProgram.mTransform = program.GetUniformLocation("transform");
			mPieceProgram.mTileSize = program.GetUniformLocation("tileSize");
			mPieceProgram.mColor = program.GetUniformLocation("pieceColor");
			mPieceProgram.mTexture = program.GetUniformLocation("colorTexture");
			mPieceProgram.mValid = (mPieceProgram.mTransform != -1 && mPieceProgram.mTileSize != -1 && mPieceProgram.mTexture != -1);
		}

		mTrailProgram.mProgramHandle = graphics.NewProgram(SHADERS_FOLDER "trail.vs", SHADERS_FOLDER "trail.fs");
		if (mTrailProgram.mProgramHandle != nullProgram) {
			const GlProgram& program = graphics.GetProgram(mTrailProgram.mProgramHandle);
			mTrailProgram.mCoords = program.GetUniformLocation("coords");
			mTrailProgram.mPerp = program.GetUniformLocation("perp");
			mTrailProgram.mColor = program.GetUniformLocation("color");
			// mTrailProgram.mTexture = program.GetUniformLocation("inputTexture");
			mTrailProgram.mValid = (mTrailProgram.mCoords != -1); // && mTrailProgram.mTexture != -1);
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
			drawCall.drawOrder = static_cast<DrawOrder>(GameDrawOrder::backgroundTile);
			drawCall.sortKey = (textureID & 255); // sort by texture
			drawCall.textures = textureIds;
			drawCall.numTextures = 1;
			drawCall.instances = instanceData;
			mGraphics.Draw(drawCall);
		}
	}

	void DrawPieces(const Board& board, const AppConfig& gameConfig, float time) const {
		if (! mPieceProgram.mValid) {
			return;
		}
		const float dynScale = 1.15f + 0.15f * std::sin(time * 8.f);
		const float cellWidth = gameConfig.board.cellWidth;
		const float cellHeight = gameConfig.board.cellHeight;
		mGraphics.SetPipeline(mPipelineBlending);

		const int uniforms[] = {
			mPieceProgram.mTransform,
			mPieceProgram.mTileSize,
			mPieceProgram.mColor,
		};

		for (const Cell& cell : board.GetCells()) {
			auto visual = static_cast<const CellVisual*>(cell.ud);
			if (visual->bitmapIdx < 0) {
				continue;
			}
			float s = visual->scale;
			if (cell.hasEffect)
				s *= dynScale;

			const float uniformData[] = { visual->coords.x + cellWidth * 0.5f,
				                          visual->coords.y + cellHeight * 0.5f,
				                          std::cos(visual->rotation),
				                          std::sin(visual->rotation), //
				                          cellWidth * s,
				                          cellHeight * s,
				                          0.f,
				                          0.f,
				                          1.f,
				                          1.f,
				                          1.f,
				                          1.f };

			//  const GLuint hrzStripesId = gameTextures[hrzStripesSprite]->GetTextureId();
			const unsigned textureIds[] = { gameTextures[visual->bitmapIdx]->GetTextureId(), 0 };

			DrawCall drawCall;
			drawCall.program = mPieceProgram.mProgramHandle;
			drawCall.mesh = quadMesh;
			drawCall.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardPiece);
			drawCall.sortKey = textureIds[0]; // sort by main texture
			drawCall.textures = textureIds;
			drawCall.numTextures = 1;
			drawCall.uniforms = uniforms;
			drawCall.uniformData = uniformData;
			drawCall.numUniforms = std::size(uniforms);
			mGraphics.Draw(drawCall);

			if (cell.layers > 0) {
				DrawIcon(cell.coords + Vec2 { cellWidth, cellHeight } * 0.5f, iceSprites[0], 0.f, whiteColor,
				         static_cast<unsigned>(GameDrawOrder::ice));
			}
		}
	}

	void DrawTrail(Vec2 start, Vec2 end, float w, float t01) const {
		if (! mTrailProgram.mValid) {
			return;
		}

		Vec2 dir = Normalize(end - start);
		Vec2 perp = Ortho(dir) * w;

		Vec2 trailStart = start;
		Vec2 trailEnd = Lerp(start, end, t01);

		mGraphics.SetPipeline(mPipelineAdditive);

		const Texture& texture = *gameTextures[glowSprite];
		const int      uniforms[] = { mTrailProgram.mCoords, mTrailProgram.mPerp, mTrailProgram.mColor };
		unsigned       textureIds[] = { texture.GetTextureId() };
		const float    uniformData[] = {
            trailStart.x, trailStart.y, trailEnd.x, trailEnd.y, //
            perp.x,       perp.y,       0.f,        0.f,        //
            1.f,          1.f,          1.f,        1.f,        // TODO Remove color ?
		};

		DrawCall drawCall;
		drawCall.program = mTrailProgram.mProgramHandle;
		drawCall.mesh = quadMesh;
		drawCall.drawOrder = static_cast<DrawOrder>(GameDrawOrder::overlays);
		drawCall.textures = textureIds;
		drawCall.numTextures = 1;
		drawCall.uniforms = uniforms;
		drawCall.uniformData = uniformData;
		drawCall.numUniforms = sizeof(uniformData) / 16;
		mGraphics.Draw(drawCall);
	}

	void DrawIcon(Vec2 coords, uint32_t iconIdx, float rotation, const Color& color, unsigned drawOrder) const {
		if (! mPieceProgram.mValid) {
			return;
		}
		mGraphics.SetPipeline(mPipelineBlending);

		const int uniforms[] = {
			mPieceProgram.mTransform,
			mPieceProgram.mTileSize,
			mPieceProgram.mColor,
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

		DrawCall drawCall;
		drawCall.program = mPieceProgram.mProgramHandle;
		drawCall.mesh = quadMesh;
		drawCall.drawOrder = drawOrder;
		drawCall.sortKey = textureIds[0]; // sort by main texture
		drawCall.textures = textureIds;
		drawCall.numTextures = 1;
		drawCall.uniforms = uniforms;
		drawCall.uniformData = uniformData;
		drawCall.numUniforms = std::size(uniforms);
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
		GLint         mPerp = 0;
		GLint         mTexture = 0;
		GLint         mColor = 0;
		bool          mValid = false;
	};

	Graphics&      mGraphics;
	PipelineHandle mPipelineBlending;
	PipelineHandle mPipelineAdditive;
	TileProgram    mTileProgram;
	PieceProgram   mPieceProgram;
	TrailProgram   mTrailProgram;
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

void GameRenderer::DrawLaser(Vec2 start, Vec2 end, float w, float t01) const {
	mPimpl->DrawTrail(start, end, w, t01);
}

void GameRenderer::DrawIcon(uint32_t iconIdx, Vec2 coords, float rotation, const Color& color, unsigned drawOrder) const {
	mPimpl->DrawIcon(coords, iconIdx, rotation, color, drawOrder);
}