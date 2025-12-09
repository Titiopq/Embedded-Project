# Real-time Scoring & Leaderboard System

A real-time scoring application built with React and Firebase that displays live game scores and allows players to submit their names to a leaderboard when the game ends.

## Features

- **Real-time Score Updates**: Automatically updates score display as it changes in Firebase
- **Game State Management**: Seamlessly transitions between "playing" and "ended" states
- **Leaderboard Submission**: Players can submit their name and score to a permanent leaderboard
- **Modern UI**: Beautiful, responsive design with Tailwind CSS
- **Secure**: Firebase security rules prevent unauthorized modifications

## Tech Stack

- **Frontend**: React 18 with Vite
- **Styling**: Tailwind CSS
- **Backend**: Firebase (Realtime Database + Firestore)
- **State Management**: React Hooks

## Project Structure

```
embeded-project/
├── src/
│   ├── components/
│   │   ├── ScoreDisplay.jsx      # Displays current score during gameplay
│   │   └── LeaderboardForm.jsx   # Form for submitting to leaderboard
│   ├── hooks/
│   │   └── useGameState.js       # Custom hook for Firebase real-time data
│   ├── firebaseConfig.js         # Firebase initialization
│   ├── App.jsx                   # Main application component
│   ├── main.jsx                  # React entry point
│   └── index.css                 # Global styles with Tailwind
├── database.rules.json           # Realtime Database security rules
├── firestore.rules               # Firestore security rules
├── package.json
└── vite.config.js
```

## Setup Instructions

### 1. Install Dependencies

```bash
npm install
```

### 2. Configure Firebase

1. Go to [Firebase Console](https://console.firebase.google.com/)
2. Create a new project (or use existing one)
3. Enable **Realtime Database** and **Cloud Firestore**

#### Set up Realtime Database:
- Go to Realtime Database section
- Create database in test mode (we'll add security rules later)
- Create a node called `current_game` with structure:
  ```json
  {
    "current_game": {
      "score": 0,
      "status": "playing"
    }
  }
  ```

#### Set up Firestore:
- Go to Firestore Database section
- Create database in test mode
- The `leaderboard` collection will be created automatically when first entry is submitted

4. Get your Firebase configuration:
   - Go to Project Settings > General
   - Scroll to "Your apps" section
   - Click the web icon (</>) to register a web app
   - Copy the configuration object

5. Update `src/firebaseConfig.js` with your actual Firebase credentials:

```javascript
const firebaseConfig = {
  apiKey: "YOUR_ACTUAL_API_KEY",
  authDomain: "YOUR_PROJECT_ID.firebaseapp.com",
  databaseURL: "https://YOUR_PROJECT_ID-default-rtdb.firebaseio.com",
  projectId: "YOUR_PROJECT_ID",
  storageBucket: "YOUR_PROJECT_ID.appspot.com",
  messagingSenderId: "YOUR_MESSAGING_SENDER_ID",
  appId: "YOUR_APP_ID"
};
```

### 3. Apply Security Rules

#### Realtime Database Rules:
1. Go to Firebase Console > Realtime Database > Rules tab
2. Copy contents from `database.rules.json` and paste them
3. Publish the rules

#### Firestore Rules:
1. Go to Firebase Console > Firestore Database > Rules tab
2. Copy contents from `firestore.rules` and paste them
3. Publish the rules

### 4. Run the Application

```bash
npm run dev
```

The app will be available at `http://localhost:5173`

## How It Works

### Data Flow

1. **Frontend watches** the `/current_game` node in Realtime Database
2. **UI updates** automatically when `score` or `status` changes
3. **When status = "playing"**: Shows large score display
4. **When status = "ended"**: Shows leaderboard submission form
5. **On form submit**: Data is saved to Firestore `leaderboard` collection

### Firebase Data Schema

#### Realtime Database (`/current_game`)
```json
{
  "score": 0,           // Current game score (number)
  "status": "playing"   // Game status: "playing" or "ended"
}
```

#### Firestore (`leaderboard` collection)
```javascript
{
  name: "Player Name",         // String
  score: 100,                  // Number
  createdAt: Timestamp         // Server timestamp
}
```

## Testing

### Test Real-time Updates
1. Open the app in your browser
2. Go to Firebase Console > Realtime Database
3. Change the `score` value manually
4. See the score update instantly in your app

### Test Game End Flow
1. In Firebase Console, change `status` from `"playing"` to `"ended"`
2. The app should show the leaderboard form
3. Enter a name and submit
4. Check Firestore Console to verify the entry was saved
5. Change status back to `"playing"` to reset

## Security

- **Realtime Database**: Frontend can only READ. Writes must come from admin/backend
- **Firestore Leaderboard**: 
  - Anyone can read
  - Anyone can create (with validation)
  - No updates or deletes allowed after submission

## Deployment

### Build for Production

```bash
npm run build
```

The build output will be in the `dist` folder.

### Deploy to Firebase Hosting (Optional)

```bash
npm install -g firebase-tools
firebase login
firebase init hosting
firebase deploy
```

## Troubleshooting

### "Permission Denied" Error
- Check that Firebase security rules are correctly applied
- Verify your Firebase configuration in `firebaseConfig.js`

### Score Not Updating
- Ensure Realtime Database URL is correct
- Check browser console for connection errors
- Verify the `/current_game` node exists in Firebase

### Form Submission Fails
- Check Firestore rules are applied
- Ensure all required fields (name, score, createdAt) are being sent
- Check browser console for detailed error messages

## License

MIT

## Contact

For questions or support, please contact the development team.

