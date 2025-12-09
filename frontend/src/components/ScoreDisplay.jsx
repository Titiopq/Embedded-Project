import React from 'react';

/**
 * ScoreDisplay Component
 * Displays the current score prominently when game is in 'playing' status
 * 
 * @param {Object} props
 * @param {number} props.score - Current score to display
 */
const ScoreDisplay = ({ score }) => {
  return (
    <div className="flex flex-col items-center justify-center space-y-6">
      <h1 className="text-6xl md:text-8xl font-bold text-white animate-pulse">
        {score}
      </h1>
      <p className="text-2xl md:text-3xl text-white/90 font-medium">
        Current Score
      </p>
      <div className="flex items-center space-x-2 text-white/70">
        <div className="w-3 h-3 bg-green-400 rounded-full animate-ping"></div>
        <span className="text-lg">Game in Progress</span>
      </div>
    </div>
  );
};

export default ScoreDisplay;

