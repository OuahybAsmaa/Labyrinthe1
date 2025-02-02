import React, { useState } from 'react';
import Grid from './Grid';
import './App.css';

const App = () => {
  const [grid, setGrid] = useState(Array(15).fill().map(() => Array(40).fill(0)));
  const [start, setStart] = useState(null); // Initialisé à null
  const [end, setEnd] = useState(null); // Initialisé à null
  const [mode, setMode] = useState('wall'); // 'wall', 'start', 'end'
  const [algorithm, setAlgorithm] = useState('dijkstra'); // 'dijkstra', 'bfs', 'dfs'
  const [isMousePressed, setIsMousePressed] = useState(false);

  const handleCellClick = (row, col) => {
    if (grid[row][col] === 4 || grid[row][col] === 2 || grid[row][col] === 3) return;

    const newGrid = [...grid];
    if (mode === 'wall') {
      newGrid[row][col] = newGrid[row][col] === 1 ? 0 : 1;
    } else if (mode === 'start') {
      if (start) newGrid[start.row][start.col] = 0;
      newGrid[row][col] = 2;
      setStart({ row, col });
      setMode('wall');
    } else if (mode === 'end') {
      if (end) newGrid[end.row][end.col] = 0;
      newGrid[row][col] = 3;
      setEnd({ row, col });
      setMode('wall');
    }
    setGrid(newGrid);
  };

  const clearPath = () => {
    return grid.map(row => row.map(cell => (cell === 4 ? 0 : cell)));
  };

  const clearEverything = () => {
    const newGrid = Array(15).fill().map(() => Array(40).fill(0));
    setGrid(newGrid);
    setStart(null); // Réinitialiser à null
    setEnd(null); // Réinitialiser à null
  };

  const handleRunAlgorithm = async () => {
    // Vérification explicite des points de départ et d'arrivée
    if (!start || !end) {
      alert('Veuillez définir à la fois un point de départ et un point d\'arrivée.');
      return;
    }

    // Si les points sont valides, continuez avec l'exécution de l'algorithme
    const clearedGrid = clearPath();
    setGrid(clearedGrid);

    await new Promise(resolve => setTimeout(resolve, 0));

    const requestData = {
      grid: clearedGrid,
      start,
      end,
      algorithm,
    };
    console.log('Grille envoyée au backend :', requestData);

    try {
      const response = await fetch('/pathfinding', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(requestData),
      });

      if (!response.ok) {
        console.error(`Erreur HTTP ! Statut : ${response.status}`);
        alert(`Erreur : Échec de la requête. Statut : ${response.status}`);
        return;
      }

      const data = await response.json();
      if (data.error) {
        alert(`Erreur du serveur : ${data.error}`);
        return;
      }
      console.log('Résultat de l algorithme :', data);

      if (!data.path || data.path.length === 0) {
        alert('Aucun chemin trouvé !');
        return;
      }

      const updatedGrid = clearedGrid.map(row => [...row]);
      data.path.forEach((point) => {
        if (point.row === undefined || point.col === undefined) {
          console.error('Point invalide dans le chemin :', point);
          return;
        }
        if (updatedGrid[point.row][point.col] !== 2 && updatedGrid[point.row][point.col] !== 3) {
          updatedGrid[point.row][point.col] = 4;
        }
      });

      setGrid(updatedGrid);
    } catch (error) {
      console.error('Erreur lors de l\'exécution de l\'algorithme :', error);
    }
  };

  const handleMouseDown = (row, col) => {
    setIsMousePressed(true);
    handleCellClick(row, col);
  };

  const handleMouseEnter = (row, col) => {
    if (isMousePressed && mode === 'wall') {
      handleCellClick(row, col);
    }
  };

  const handleMouseUp = () => {
    setIsMousePressed(false);
  };

  return (
    <div className="App">
      <h1>Cheminement Labyrinthe</h1>
      <div className="controls">
        <button onClick={() => setMode('start')}>Point de départ</button>
        <button onClick={() => setMode('end')}>Point d'arrivée</button>
        <button onClick={clearEverything}>Tout effacer</button>
        <select
          value={algorithm}
          onChange={(e) => setAlgorithm(e.target.value)}
          className="algorithm-select"
        >
          <option value="dijkstra">Dijkstra</option>
          <option value="bfs">BFS</option>
          <option value="dfs">DFS</option>
        </select>
        <button onClick={handleRunAlgorithm}>Lancer l'algorithme</button>
      </div>
      <div className="grid-container">
        <Grid
          rows={15}
          cols={40}
          cellSize={25}
          grid={grid}
          onCellClick={handleCellClick}
          onMouseDown={handleMouseDown}
          onMouseEnter={handleMouseEnter}
          onMouseUp={handleMouseUp}
        />
      </div>
    </div>
  );
};

export default App;