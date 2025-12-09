import React, { useState } from 'react';
import { collection, addDoc, serverTimestamp } from 'firebase/firestore';
import { firestore } from '../firebaseConfig';

/**
 * LeaderboardForm Component
 * Displays when game status is 'ended'
 * Allows player to submit their name and score to the leaderboard
 * 
 * @param {Object} props
 * @param {number} props.score - Final score to be submitted
 * @param {boolean} props.isSubmitted - Whether the form has been submitted already
 * @param {Function} props.onSubmitSuccess - Callback function after successful submission
 */
const LeaderboardForm = ({ score, isSubmitted, onSubmitSuccess }) => {
  const [name, setName] = useState('');
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  const handleSubmit = async (e) => {
    e.preventDefault();
    
    // Validation
    if (!name.trim()) {
      setError('Please enter your name');
      return;
    }

    setLoading(true);
    setError(null);

    try {
      // Prepare payload
      const payload = {
        name: name.trim(),
        score: score,
        createdAt: serverTimestamp()
      };

      // Submit to Firestore
      const leaderboardRef = collection(firestore, 'leaderboard');
      await addDoc(leaderboardRef, payload);

      // Success
      setName('');
      onSubmitSuccess();
    } catch (err) {
      console.error('Error submitting to leaderboard:', err);
      setError('Failed to submit. Please try again.');
    } finally {
      setLoading(false);
    }
  };

  // Show thank you message after submission
  if (isSubmitted) {
    return (
      <div className="flex flex-col items-center justify-center space-y-6">
        <div className="text-6xl">🎉</div>
        <h2 className="text-4xl md:text-5xl font-bold text-white">
          Thank You!
        </h2>
        <p className="text-xl text-white/90">
          Your score has been saved to the leaderboard
        </p>
        <div className="mt-4 p-4 bg-white/10 rounded-lg backdrop-blur-sm text-center">
          <p className="text-white/70">Final Score:</p>
          <p className="text-5xl font-bold text-white">{score}</p>
        </div>
      </div>
    );
  }

  // Show form
  return (
    <div className="flex flex-col items-center justify-center space-y-6 w-full max-w-md px-4 mx-auto">
      <div className="text-center space-y-4">
        <h2 className="text-5xl md:text-6xl font-bold text-white">
          Game Over
        </h2>
        <p className="text-2xl text-white/90">
          Final Score
        </p>
        <div className="text-6xl md:text-7xl font-bold text-cyan-300 drop-shadow-lg">
          {score}
        </div>
      </div>

      <form onSubmit={handleSubmit} className="w-full space-y-4 mt-8">
        <div>
          <label htmlFor="name" className="block text-white text-lg mb-2">
            Enter Your Name
          </label>
          <input
            type="text"
            id="name"
            value={name}
            onChange={(e) => setName(e.target.value)}
            disabled={loading}
            placeholder="Your name"
            className="w-full px-4 py-3 text-lg rounded-lg bg-white/90 text-gray-800 
                     placeholder-gray-400 focus:outline-none focus:ring-4 
                     focus:ring-cyan-300 disabled:opacity-50 disabled:cursor-not-allowed"
            maxLength={50}
          />
        </div>

        {error && (
          <div className="p-3 bg-red-500/20 border border-red-500 rounded-lg">
            <p className="text-red-100 text-sm">{error}</p>
          </div>
        )}

        <button
          type="submit"
          disabled={loading}
          className="w-full py-4 text-xl font-bold rounded-lg bg-gradient-to-r from-cyan-400 to-teal-400 
                   text-white hover:from-cyan-300 hover:to-teal-300 transition-all transform hover:scale-105
                   disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:scale-100
                   focus:outline-none focus:ring-4 focus:ring-cyan-300 shadow-lg"
        >
          {loading ? 'Submitting...' : 'Submit to Leaderboard'}
        </button>
      </form>

      <p className="text-white/60 text-sm text-center mt-4">
        Your score will be permanently saved
      </p>
    </div>
  );
};

export default LeaderboardForm;

