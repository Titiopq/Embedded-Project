import React from 'react';

/**
 * LandingPage Component
 * Displays a welcoming landing page when game is in 'idle' status
 * Shows that the system is ready and waiting for a game to start
 */
const LandingPage = () => {
  return (
    <div className="flex flex-col items-center justify-center space-y-8">
      <div className="text-center space-y-4">
        <h1 className="text-5xl md:text-7xl font-bold text-white mb-4">
          Welcome! 👋
        </h1>
        <p className="text-2xl md:text-3xl text-white/90 font-medium">
          Waiting for game to start...
        </p>
      </div>
      
      <div className="flex items-center space-x-3 text-white/70 mt-8">
        <div className="relative">
          <div className="w-4 h-4 bg-yellow-400 rounded-full animate-pulse"></div>
          <div className="absolute inset-0 w-4 h-4 bg-yellow-400 rounded-full animate-ping"></div>
        </div>
        <span className="text-lg">System Ready</span>
      </div>

      <div className="mt-12 text-center space-y-2">
        <p className="text-white/60 text-sm">
          The game will begin shortly
        </p>
        <p className="text-white/50 text-xs">
          Keep this page open and wait for the game to start
        </p>
      </div>
    </div>
  );
};

export default LandingPage;

