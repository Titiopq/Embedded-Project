# Quick Start Guide

## 📦 Installation

```bash
npm install
```

## ⚙️ Configuration

1. **Update Firebase Config** in `src/firebaseConfig.js`

   - Replace all `YOUR_*` placeholders with actual Firebase credentials
   - Get credentials from: Firebase Console > Project Settings > Your apps

2. **Set up Firebase Data Structure**

   In Realtime Database, create:

   ```
   /current_game
     - score: 0
     - status: "playing"
   ```

3. **Apply Security Rules**
   - Copy `database.rules.json` → Firebase Console > Realtime Database > Rules
   - Copy `firestore.rules` → Firebase Console > Firestore Database > Rules

## 🚀 Run

```bash
npm run dev
```

Open: http://localhost:5173

## 🧪 Test

1. **Change score**: Firebase Console > Realtime Database > current_game > score → Watch it update live
2. **End game**: Change status to "ended" → Form appears
3. **Submit**: Enter name and submit → Check Firestore leaderboard collection
4. **Reset**: Change status back to "playing"

## 📚 Documentation

- Full docs: `README.md` (English)
- คู่มือภาษาไทย: `SETUP_TH.md` (Thai)

## 🏗️ Build

```bash
npm run build
```

Output: `dist/` folder

---

**Need Help?** Check README.md for detailed troubleshooting.
