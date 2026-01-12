#include "GameRenderer.h"
#include "AssetDefs.h"
#include "Board.h"
#include "GameConfig.h"
#include "GameDrawOrder.h"

#include <engine/Engine.h>
#include <engine/GlProgram.h>
#include <engine/Graphics.h>

using namespace Wind;

struct TileProgram {
	ProgramHandle mProgramHandle = nullProgram;
	GLint         mColor = 0;
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
			mTileProgram.mColor = program.GetUniformLocation("color");
			mTileProgram.mCoords = program.GetUniformLocation("coords");
			mTileProgram.mTexture = program.GetUniformLocation("inputTexture");
			mTileProgram.mValid = (mTileProgram.mColor != -1 && mTileProgram.mCoords != -1 && mTileProgram.mTexture != -1);
		}

		mPieceProgram.mProgramHandle = graphics.NewProgram(SHADERS_FOLDER "piece.vs", SHADERS_FOLDER "piece.fs");
		if (mPieceProgram.mProgramHandle != nullProgram) {
			const GlProgram& program = graphics.GetProgram(mPieceProgram.mProgramHandle);
			mPieceProgram.mColor = program.GetUniformLocation("color");
			mPieceProgram.mCoords = program.GetUniformLocation("coords");
			mPieceProgram.mTexture = program.GetUniformLocation("inputTexture");
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
		const int           uniforms[] = { mTileProgram.mCoords, mTileProgram.mColor };
		const unsigned      textureIds[] = { textureID };

		DrawCall drawCall;
		drawCall.program = mTileProgram.mProgramHandle;
		drawCall.mesh = quadMesh;
		drawCall.drawOrder = static_cast<DrawOrder>(GameDrawOrder::backgroundTile);
		drawCall.sortKey = (textureID & 255); // sort by texture
		drawCall.textures = textureIds;
		drawCall.numTextures = 1;
		drawCall.uniforms = uniforms;

		for (const Cell& cell : board.GetCells()) {
			if (cell.category != CellCategory::hole) {
				float left = cell.coords.x - cellSpacing;
				float top = cell.coords.y - cellSpacing;
				const float uniformData[][4] = {
					{ left, top, cellWidth + 2 * cellSpacing, cellHeight + 2 * cellSpacing },
					{ 1.f, 1.f, 1.f, 140.f / 255.f },
				};

				drawCall.uniformData = uniformData;
				drawCall.numUniforms = sizeof(uniformData) / 16;
				mGraphics.Draw(drawCall);
			}
		}
	}

	void DrawPieces(const Board& board, const GameConfig& gameConfig) const {
		if (! mPieceProgram.mValid) {
			return;
		}
		const float cellWidth = gameConfig.board.cellWidth;
		const float cellHeight = gameConfig.board.cellHeight; // FIXME Store in Cell
		mGraphics.SetPipeline(mPipelineBlending);

		const int uniforms[] = { mPieceProgram.mCoords, mPieceProgram.mColor };

		DrawCall drawCall;
		drawCall.program = mPieceProgram.mProgramHandle;
		drawCall.mesh = quadMesh;
		drawCall.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardPiece);
		drawCall.numTextures = 1;
		drawCall.uniforms = uniforms;

		for (const Cell& cell : board.GetCells()) {
			if (cell.pieceGraphics.bitmapIdx < 0) {
				continue;
			}

			const Texture& texture = *sprites[cell.pieceGraphics.bitmapIdx];
			const unsigned textureIds[] = { texture.GetTextureId() };

			float w = cellWidth * cell.pieceGraphics.scale;
			float h = cellHeight * cell.pieceGraphics.scale;
			float left = cell.pieceGraphics.coords.x + cellWidth * 0.5f - w * 0.5f;
			float top = cell.pieceGraphics.coords.y + cellHeight * 0.5f - h * 0.5f;

			const float uniformData[][4] = {
				{ left, top, w, h },
				{ 1.f, 1.f, 1.f, 1.f }, // TODO Remove ?
			};

			drawCall.sortKey = (texture.GetTextureId() & 255); // sort by texture
			drawCall.textures = textureIds;
			drawCall.uniformData = uniformData;
			drawCall.numUniforms = sizeof(uniformData) / 16;
			mGraphics.Draw(drawCall);
		}
	}

private:
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

#if 0
	// Draw pieces, obstacles and boosters
	for (const Cell& cell : board.GetCells()) {
		if (cell.hasBooster) {
			// TODO REmove, draw different bitmap
			prm.scale.x = 0.5f + 0.1f * dynScaleFactor;
			prm.scale.y = 0.5f + 0.1f * dynScaleFactor;
			prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::ice);
			prm.orientation = 0.f;
			pos.x += cellWidth * 0.25f;
			pos.y += cellHeight * 0.25f;
			bitmapRender.DrawBitmapEx(*sprites[boosterDefs[(int)cell.boosterType].sprite], pos, prm);
		}
	}
	// Highlight selected cell
	if (selectedCell >= 0) {
		const Cell&     cell = board.GetCell(selectedCell);
		BitmapExtParams prm;
		prm.width = cellWidth + 2.f * cellSpacing;
		prm.height = cellHeight + 2.f * cellSpacing;
		prm.pivot = BitmapPivot::topLeft;
		prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardTile);
		prm.blending = true;
		bitmapRender.DrawBitmapEx(*sprites[selectionSprite], cell.pieceGraphics.coords - Vec2 { cellSpacing, cellSpacing }, prm);
	}
#endif
}