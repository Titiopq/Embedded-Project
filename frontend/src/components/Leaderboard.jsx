import React, { useState, useEffect } from 'react';
import { collection, query, orderBy, limit, getDocs } from 'firebase/firestore';
import { firestore } from '../firebaseConfig';

/**
 * Leaderboard Component
 * Displays top scores from Firestore in a ranked table
 * 
 * @param {Object} props
 * @param {Function} props.onClose - Callback to close the leaderboard view
 */
const Leaderboard = ({ onClose }) => {
  const [entries, setEntries] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    fetchLeaderboard();
  }, []);

  const fetchLeaderboard = async () => {
    try {
      setLoading(true);
      setError(null);

      // Query Firestore for top 10 scores
      const leaderboardRef = collection(firestore, 'leaderboard');
      const q = query(
        leaderboardRef,
        orderBy('score', 'desc'),
        limit(10)
      );

      const querySnapshot = await getDocs(q);
      const data = [];
      
      querySnapshot.forEach((doc) => {
        data.push({
          id: doc.id,
          ...doc.data()
        });
      });

      setEntries(data);
    } catch (err) {
      console.error('Error fetching leaderboard:', err);
      setError('Failed to load leaderboard');
    } finally {
      setLoading(false);
    }
  };

  const getMedalEmoji = (index) => {
    switch (index) {
      case 0: return '🥇';
      case 1: return '🥈';
      case 2: return '🥉';
      default: return null;
    }
  };

  return (
    <div className="fixed inset-0 bg-black/60 backdrop-blur-md flex items-center justify-center p-4 z-50 animate-fade-in">
      <div className="bg-gradient-to-br from-blue-500 to-purple-600 rounded-3xl shadow-2xl max-w-3xl w-full max-h-[95vh] overflow-hidden border-2 border-white/20">
        {/* Header */}
        <div className="relative bg-gradient-to-r from-blue-400/20 via-white/10 to-purple-400/20 backdrop-blur-sm px-8 py-6 border-b-2 border-white/30">
          <div className="flex items-center justify-between">
            <div className="flex items-center space-x-4">
              <div className="relative">
                <span className="text-5xl animate-bounce">🏆</span>
                <div className="absolute -inset-1 bg-cyan-400/30 rounded-full blur-xl"></div>
              </div>
              <div>
                <h2 className="text-4xl font-black text-white tracking-tight">
                  Leaderboard
                </h2>
                <p className="text-white/70 text-sm mt-1">Top Players Rankings</p>
              </div>
            </div>
            <button
              onClick={onClose}
              className="text-white/70 hover:text-white hover:bg-white/20 rounded-xl p-2.5 transition-all hover:rotate-90 duration-300"
              aria-label="Close"
            >
              <svg className="w-7 h-7" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2.5} d="M6 18L18 6M6 6l12 12" />
              </svg>
            </button>
          </div>
        </div>

        {/* Content */}
        <div className="p-6 overflow-y-auto max-h-[calc(95vh-160px)] custom-scrollbar">
          {loading ? (
            <div className="text-center py-16">
              <div className="relative inline-block">
                <div className="animate-spin rounded-full h-16 w-16 border-t-4 border-b-4 border-cyan-400"></div>
                <div className="absolute inset-0 animate-ping rounded-full h-16 w-16 border-4 border-cyan-400 opacity-20"></div>
              </div>
              <p className="text-white/90 text-lg mt-6 font-semibold">Loading rankings...</p>
            </div>
          ) : error ? (
            <div className="text-center py-16">
              <div className="text-6xl mb-4">⚠️</div>
              <p className="text-red-300 text-xl mb-6 font-semibold">{error}</p>
              <button
                onClick={fetchLeaderboard}
                className="px-8 py-3 bg-gradient-to-r from-cyan-500 to-blue-500 hover:from-cyan-400 hover:to-blue-400 rounded-xl text-white font-semibold transition-all transform hover:scale-105 shadow-lg"
              >
                Try Again
              </button>
            </div>
          ) : entries.length === 0 ? (
            <div className="text-center py-16">
              <div className="text-7xl mb-4">🎮</div>
              <p className="text-white/80 text-xl font-semibold">No entries yet</p>
              <p className="text-white/60 mt-2">Be the first champion!</p>
            </div>
          ) : (
            <div className="space-y-3">
              {entries.map((entry, index) => {
                const isTop3 = index < 3;
                const medal = getMedalEmoji(index);
                
                return (
                  <div
                    key={entry.id}
                    style={{ animationDelay: `${index * 50}ms` }}
                    className={`relative flex items-center gap-4 p-5 rounded-2xl transition-all duration-300 hover:scale-[1.02] animate-slide-up ${
                      index === 0
                        ? 'bg-gradient-to-r from-yellow-500/30 via-yellow-400/20 to-yellow-500/30 border-2 border-yellow-400 shadow-xl shadow-yellow-500/20'
                        : index === 1
                        ? 'bg-gradient-to-r from-gray-400/30 via-gray-300/20 to-gray-400/30 border-2 border-gray-300 shadow-xl shadow-gray-400/20'
                        : index === 2
                        ? 'bg-gradient-to-r from-orange-500/30 via-orange-400/20 to-orange-500/30 border-2 border-orange-400 shadow-xl shadow-orange-500/20'
                        : 'bg-white/10 backdrop-blur-sm border-2 border-white/20 hover:border-white/40'
                    }`}
                  >
                    {/* Rank Badge */}
                    <div className="flex-shrink-0">
                      {isTop3 ? (
                        <div className="relative">
                          <div className={`text-6xl ${index === 0 ? 'animate-pulse' : ''}`}>
                            {medal}
                          </div>
                          {index === 0 && (
                            <div className="absolute inset-0 bg-yellow-400/30 rounded-full blur-xl animate-pulse"></div>
                          )}
                        </div>
                      ) : (
                        <div className="w-14 h-14 rounded-full bg-gradient-to-br from-white/20 to-white/5 border-2 border-white/30 flex items-center justify-center">
                          <span className="text-white font-black text-xl">#{index + 1}</span>
                        </div>
                      )}
                    </div>

                    {/* Player Info */}
                    <div className="flex-1 min-w-0">
                      <div className="flex items-center gap-2 mb-1">
                        <p className={`font-bold text-xl truncate ${
                          isTop3 ? 'text-white' : 'text-white/90'
                        }`}>
                          {entry.name}
                        </p>
                        {index === 0 && <span className="text-xl">👑</span>}
                      </div>
                      {entry.createdAt && (
                        <p className="text-white/50 text-xs font-medium">
                          {new Date(entry.createdAt.toDate()).toLocaleDateString('th-TH', {
                            month: 'short',
                            day: 'numeric',
                            hour: '2-digit',
                            minute: '2-digit'
                          })}
                        </p>
                      )}
                    </div>

                    {/* Score */}
                    <div className="text-right flex-shrink-0">
                      <div className={`text-4xl font-black tracking-tight ${
                        index === 0 ? 'text-yellow-300' :
                        index === 1 ? 'text-gray-200' :
                        index === 2 ? 'text-orange-300' :
                        'text-white'
                      }`}>
                        {entry.score.toLocaleString()}
                      </div>
                      <p className="text-white/60 text-xs font-semibold uppercase tracking-wider mt-1">
                        points
                      </p>
                    </div>

                    {/* Glow effect for top 3 */}
                    {isTop3 && (
                      <div className={`absolute inset-0 rounded-2xl blur-xl opacity-20 -z-10 ${
                        index === 0 ? 'bg-yellow-400 animate-pulse' :
                        index === 1 ? 'bg-gray-300' :
                        'bg-orange-400'
                      }`}></div>
                    )}
                  </div>
                );
              })}
            </div>
          )}
        </div>

        {/* Footer */}
        <div className="bg-gradient-to-r from-blue-600/40 via-purple-600/40 to-blue-600/40 backdrop-blur-sm px-8 py-5 border-t-2 border-white/30">
          <button
            onClick={onClose}
            className="w-full py-4 bg-gradient-to-r from-blue-500 to-purple-500 hover:from-blue-400 hover:to-purple-400 rounded-xl text-white text-lg font-bold transition-all transform hover:scale-[1.02] shadow-lg hover:shadow-xl"
          >
            Close
          </button>
        </div>
      </div>
    </div>
  );
};

export default Leaderboard;

