#include "GameRenderer.h"
#include "AssetDefs.h"
#include "Board.h"
#include "GameConfig.h"
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
			mPieceProgram.mColor = program.GetUniformLocation("color");
			mPieceProgram.mCoords = program.GetUniformLocation("coords");
			mPieceProgram.mTexture = program.GetUniformLocation("colorTexture");
			mPieceProgram.mValid = (mPieceProgram.mColor != -1 && mPieceProgram.mCoords != -1 && mPieceProgram.mTexture != -1);
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

		for (int i = 0; i < NumSprites; ++i) {
			sprites[i] = graphics.LoadTexture(spriteDefs[i].bitmap);
		}
	}

	void DrawBackgroundTiles(const Board& board, const GameConfig& gameConfig) const {
		if (! mTileProgram.mValid) {
			return;
		}
		const float cellWidth = gameConfig.board.cellWidth;
		const float cellHeight = gameConfig.board.cellHeight;
		const float cellSpacing = gameConfig.board.cellSpacing;
		mGraphics.SetPipeline(mPipelineBlending);

		const GLuint   textureID = sprites[boardTileDefs[0]]->GetTextureId();
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

	void DrawPieces(const Board& board, const GameConfig& gameConfig) const {
		if (! mPieceProgram.mValid) {
			return;
		}
		const float cellWidth = gameConfig.board.cellWidth;
		const float cellHeight = gameConfig.board.cellHeight; // FIXME Store in Cell
		mGraphics.SetPipeline(mPipelineBlending);

		const int    uniforms[] = { mPieceProgram.mCoords, mPieceProgram.mColor };
		const GLuint hrzStripesId = sprites[hrzStripesSprite]->GetTextureId();
		unsigned     textureIds[] = { 0, 0 };

		DrawCall drawCall;
		drawCall.program = mPieceProgram.mProgramHandle;
		drawCall.mesh = quadMesh;
		drawCall.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardPiece);
		drawCall.textures = textureIds;
		drawCall.uniforms = uniforms;

		// int pieceCount[MaxPieceTypes] {};

		// TODO Instanced
		for (const Cell& cell : board.GetCells()) {
			auto tileVisual = static_cast<const CellVisual*>(cell.ud);
			if (tileVisual->bitmapIdx < 0) {
				continue;
			}

			const Texture& texture = *sprites[tileVisual->bitmapIdx];
			float          s = tileVisual->scale;
			if (cell.hasEffect)
				s *= 1.1f;
			float w = cellWidth * s;
			float h = cellHeight * s;
			float left = tileVisual->coords.x + cellWidth * 0.5f - w * 0.5f;
			float top = tileVisual->coords.y + cellHeight * 0.5f - h * 0.5f;

			const float uniformData[][4] = {
				{ left, top, w, h }, { 1.f, 1.f, 1.f, 1.f }, // TODO Remove ?
			};
			textureIds[0] = texture.GetTextureId();
			textureIds[1] = cell.hasEffect ? hrzStripesId : 0;
			drawCall.numTextures = 2;
			drawCall.sortKey = (texture.GetTextureId() & 255); // sort by main texture
			drawCall.uniformData = uniformData;
			drawCall.numUniforms = sizeof(uniformData) / 16;
			mGraphics.Draw(drawCall);
		}
	}

	void DrawTrail(Wind::Vec2 start, Wind::Vec2 end, float w, float t01) const {
		if (! mTrailProgram.mValid) {
			return;
		}

		Vec2 dir = Normalize(end - start);
		Vec2 perp = Ortho(dir) * w;
		
		const float th = 0.1f;
		Vec2 trailStart = start;
		Vec2 trailEnd = Lerp(start, end, th + (1.f - th) * t01);

		mGraphics.SetPipeline(mPipelineAdditive);

		const Texture& texture = *sprites[glowSprite];
		const int      uniforms[] = { mTrailProgram.mCoords, mTrailProgram.mPerp, mTrailProgram.mColor };
		unsigned       textureIds[] = { texture.GetTextureId() };
		const float    uniformData[] = {
            trailStart.x, trailStart.y, trailEnd.x, trailEnd.y, //
            perp.x,  perp.y,  0.f,   0.f,   //
            1.f,     1.f,     1.f,   1.f,   // TODO Remove color ?
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

private:
	struct TileProgram {
		ProgramHandle mProgramHandle = nullProgram;
		GLint         mCoords = 0;
		GLint         mTexture = 0;
		bool          mValid = false;
	};

	struct PieceProgram {
		ProgramHandle mProgramHandle = nullProgram;
		GLint         mColor = 0;
		GLint         mCoords = 0;
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

GameRenderer::GameRenderer(Wind::Engine& engine)
    : mPimpl { std::make_unique<Impl>(engine.GetGraphics()) } {
}

GameRenderer::~GameRenderer() = default;

void GameRenderer::DrawBoard(const Board& board, int selectedCell, const GameConfig& gameConfig) const {
	mPimpl->DrawBackgroundTiles(board, gameConfig);
	mPimpl->DrawPieces(board, gameConfig);
}

void GameRenderer::DrawLaser(Wind::Vec2 start, Wind::Vec2 end, float w, float t01) const {
	mPimpl->DrawTrail(start, end, w, t01);
}
