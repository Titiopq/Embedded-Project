import { useState, useEffect } from "react";
import { ref, onValue } from "firebase/database";
import { database } from "../firebaseConfig";

/**
 * Custom hook to watch game state from Firebase Realtime Database
 * Listens to /current_game path and manages local state
 *
 * @returns {Object} gameState - Current game state
 * @returns {number} gameState.score - Current score
 * @returns {string} gameState.status - Game status ('idle', 'playing', or 'ended')
 * @returns {boolean} gameState.isFormSubmitted - Whether form has been submitted
 * @returns {boolean} gameState.loading - Loading state
 * @returns {string|null} gameState.error - Error message if any
 * @returns {Function} setIsFormSubmitted - Function to update form submission state
 */
export const useGameState = () => {
  const [score, setScore] = useState(0);
  const [status, setStatus] = useState("playing");
  const [isFormSubmitted, setIsFormSubmitted] = useState(false);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    // Create a reference to the /current_game path
    const gameRef = ref(database, "current_game");

    // Subscribe to changes
    const unsubscribe = onValue(
      gameRef,
      (snapshot) => {
        const data = snapshot.val();

        if (data) {
          // Update score if exists
          if (typeof data.score === "number") {
            setScore(data.score);
          }

          // Update status if exists
          if (data.status) {
            setStatus(data.status);

            // Reset form submission state when game restarts (status changes to 'playing')
            if (data.status === "playing") {
              setIsFormSubmitted(false);
            }
          }

          setError(null);
        } else {
          // No data exists yet
          setScore(0);
          setStatus("playing");
        }

        setLoading(false);
      },
      (error) => {
        console.error("Firebase error:", error);
        setError(error.message);
        setLoading(false);
      }
    );

    // Cleanup subscription on unmount
    return () => unsubscribe();
  }, []);

  return {
    score,
    status,
    isFormSubmitted,
    setIsFormSubmitted,
    loading,
    error,
  };
};
