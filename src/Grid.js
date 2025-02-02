import React from 'react';
import { Stage, Layer, Rect } from 'react-konva';

const Grid = ({
  rows,
  cols,
  cellSize,
  grid,
  onCellClick,
  onMouseDown,
  onMouseEnter,
  onMouseUp,
}) => {
  return (
    <Stage width={cols * cellSize} height={rows * cellSize}>
      <Layer>
        {grid.map((row, rowIndex) =>
          row.map((cell, colIndex) => {
            let fillColor = 'white'; // Par défaut, cellule vide
            if (cell === 1) fillColor = 'black'; // Mur
            if (cell === 2) fillColor = 'green'; // Départ
            if (cell === 3) fillColor = 'red'; // Arrivée
            if (cell === 4) {
              // Chemin : couleur bleue
              fillColor = 'LightSkyBlue'; // Bleu
            }

            return (
              <Rect
                key={`${rowIndex}-${colIndex}`}
                x={colIndex * cellSize}
                y={rowIndex * cellSize}
                width={cellSize}
                height={cellSize}
                fill={fillColor}
                stroke="black" // Couleur de la bordure
                strokeWidth={1} // Épaisseur de la bordure en 1px
                onClick={() => onCellClick(rowIndex, colIndex)}
                onMouseDown={() => onMouseDown(rowIndex, colIndex)}
                onMouseEnter={() => onMouseEnter(rowIndex, colIndex)}
                onMouseUp={onMouseUp}
                shadowColor={cell === 4 ? '#87CEFA' : null} // Ombre bleue pour le chemin
                shadowBlur={cell === 4 ? 10 : 0} // Effet de brillance
                shadowOpacity={cell === 4 ? 0.8 : 0}
              />
            );
          })
        )}
      </Layer>
    </Stage>
  );
};

export default Grid;