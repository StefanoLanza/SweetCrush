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
		mPipelineBlending = graphics.NewPipeline(pipelineState);

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
	}

	void DrawBackgroundTiles(const Board& board, const GameConfig& gameConfig) const {
		if (! mTileProgram.mValid) {
			return;
		}
		const float cellWidth = gameConfig.board.cellWidth;
		const float cellHeight = gameConfig.board.cellHeight;
		const float cellSpacing = gameConfig.board.cellSpacing;
		mGraphics.SetPipeline(mPipelineBlending);

		const BoardTileDef& def = boardTileDefs[0];
		const GLuint        textureID = sprites[def.sprite]->GetTextureId();
		const unsigned      textureIds[] = { textureID };

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
				tiles[idx].coords = { cell.coords.x - cellSpacing, cell.coords.y - cellSpacing, cellWidth + 2 * cellSpacing,
					                  cellHeight + 2 * cellSpacing };
				tiles[idx].color = { 1.f, 1.f, 1.f, 140.f / 255.f };
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
			if (cell.pieceGraphics.bitmapIdx < 0) {
				continue;
			}

			const Texture& texture = *sprites[cell.pieceGraphics.bitmapIdx];
			float          s = cell.pieceGraphics.scale;
			if (cell.hasEffect)
				s *= 1.1f;
			float w = cellWidth * s;
			float h = cellHeight * s;
			float left = cell.pieceGraphics.coords.x + cellWidth * 0.5f - w * 0.5f;
			float top = cell.pieceGraphics.coords.y + cellHeight * 0.5f - h * 0.5f;

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

	Graphics&      mGraphics;
	PipelineHandle mPipelineBlending;
	TileProgram    mTileProgram;
	PieceProgram   mPieceProgram;
};

GameRenderer::GameRenderer(Wind::Engine& engine)
    : mPimpl { std::make_unique<Impl>(engine.GetGraphics()) } {
}

GameRenderer::~GameRenderer() = default;

void GameRenderer::DrawBoard(const Board& board, int selectedCell, const GameConfig& gameConfig) const {
	mPimpl->DrawBackgroundTiles(board, gameConfig);
	mPimpl->DrawPieces(board, gameConfig);
}