import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";
import { getFirestore } from "firebase/firestore";

// TODO: Replace with your actual Firebase configuration
// Get these values from Firebase Console > Project Settings > General > Your apps
const firebaseConfig = {
  apiKey: "AIzaSyC8AqtEShzw2Oic6bFVAsMCXx0Vl_C95Kc",
  authDomain: "scoring-system-embeded.firebaseapp.com",
  databaseURL:
    "https://scoring-system-embeded-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "scoring-system-embeded",
  storageBucket: "scoring-system-embeded.firebasestorage.app",
  messagingSenderId: "162010224224",
  appId: "1:162010224224:web:1c283eb5ad200c41a79505",
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Initialize Realtime Database and get a reference to the service
export const database = getDatabase(app);

// Initialize Firestore and get a reference to the service
export const firestore = getFirestore(app);
