import React, { useState } from "react";
import { useGameState } from "./hooks/useGameState";
import ScoreDisplay from "./components/ScoreDisplay";
import LeaderboardForm from "./components/LeaderboardForm";
import Leaderboard from "./components/Leaderboard";
import LandingPage from "./components/LandingPage";

function App() {
  const { score, status, isFormSubmitted, setIsFormSubmitted, loading, error } =
    useGameState();
  const [showLeaderboard, setShowLeaderboard] = useState(false);

  // Handle loading state
  if (loading) {
    return (
      <div className="min-h-screen bg-gradient-to-br from-blue-500 to-purple-600 flex items-center justify-center">
        <div className="text-center space-y-4">
          <div className="animate-spin rounded-full h-16 w-16 border-b-4 border-white mx-auto"></div>
          <p className="text-white text-xl">Connecting...</p>
        </div>
      </div>
    );
  }

  // Handle error state
  if (error) {
    return (
      <div className="min-h-screen bg-gradient-to-br from-red-500 to-pink-600 flex items-center justify-center">
        <div className="bg-white/10 backdrop-blur-sm rounded-lg p-8 max-w-md mx-4">
          <h2 className="text-white text-2xl font-bold mb-4">
            Connection Error
          </h2>
          <p className="text-white/90 mb-4">{error}</p>
          <p className="text-white/70 text-sm">
            Please check your Firebase configuration and make sure the Realtime
            Database is set up correctly.
          </p>
        </div>
      </div>
    );
  }

  // Render based on game status
  return (
    <div className="min-h-screen bg-gradient-to-br from-blue-500 to-purple-600 flex items-center justify-center p-4">
      <div className="container mx-auto">
        {status === "idle" ? (
          <LandingPage />
        ) : status === "playing" ? (
          <ScoreDisplay score={score} />
        ) : (
          <LeaderboardForm
            score={score}
            isSubmitted={isFormSubmitted}
            onSubmitSuccess={() => setIsFormSubmitted(true)}
          />
        )}
      </div>

      {/* Leaderboard Button */}
      <button
        onClick={() => setShowLeaderboard(true)}
        className="fixed top-6 right-6 bg-white/20 hover:bg-white/30 backdrop-blur-sm text-white px-6 py-3 rounded-full font-semibold shadow-lg transition-all transform hover:scale-105 flex items-center space-x-2"
      >
        <span className="text-2xl">🏆</span>
        <span>Leaderboard</span>
      </button>

      {/* Footer */}
      <div className="fixed bottom-4 left-0 right-0 text-center">
        <p className="text-white/50 text-sm">
          Real-time Scoring & Leaderboard System
        </p>
      </div>

      {/* Leaderboard Modal */}
      {showLeaderboard && (
        <Leaderboard onClose={() => setShowLeaderboard(false)} />
      )}
    </div>
  );
}

export default App;
