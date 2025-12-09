# คู่มือการติดตั้ง (Setup Guide - ภาษาไทย)

## ขั้นตอนการติดตั้งและรันโปรเจกต์

### 1. ติดตั้ง Dependencies

```bash
npm install
```

### 2. ตั้งค่า Firebase

#### สร้าง Firebase Project

1. ไปที่ [Firebase Console](https://console.firebase.google.com/)
2. คลิก "Add project" หรือ "เพิ่มโปรเจกต์"
3. ตั้งชื่อโปรเจกต์ (เช่น "scoring-system")
4. ทำตามขั้นตอนจนเสร็จ

#### เปิดใช้งาน Realtime Database

1. เลือกเมนู "Realtime Database" จากแถบด้านซ้าย
2. คลิก "Create Database"
3. เลือก Location ที่ใกล้ที่สุด (เช่น asia-southeast1)
4. เริ่มต้นด้วย "Test mode" ก่อน (เราจะตั้งค่า Security Rules ทีหลัง)
5. หลังจากสร้างเสร็จ ให้สร้าง node ชื่อ `current_game`:
   - คลิกที่ + ถัดจาก root
   - ใส่ชื่อ: `current_game`
   - เพิ่ม child nodes:
     - `score`: 0 (type: number)
     - `status`: "playing" (type: string)

#### เปิดใช้งาน Firestore

1. เลือกเมนู "Firestore Database"
2. คลิก "Create database"
3. เริ่มต้นด้วย "Test mode"
4. เลือก Location เดียวกับ Realtime Database
5. Collection `leaderboard` จะถูกสร้างอัตโนมัติเมื่อมีการส่งข้อมูลครั้งแรก

#### ดึง Configuration Keys

1. ไปที่ Project Settings (ไอคอนเฟืองข้างๆ Project Overview)
2. เลื่อนลงมาที่ "Your apps"
3. คลิกไอคอน Web (</>) เพื่อสร้าง Web App
4. ตั้งชื่อแอพ (เช่น "scoring-web")
5. คัดลอก Config Object

#### อัปเดต firebaseConfig.js

1. เปิดไฟล์ `src/firebaseConfig.js`
2. แทนที่ค่า placeholder ด้วยค่าจริงที่คัดลอกมา:

```javascript
const firebaseConfig = {
  apiKey: "AIza...", // ใส่ค่าจริง
  authDomain: "your-project.firebaseapp.com",
  databaseURL: "https://your-project-default-rtdb.firebaseio.com",
  projectId: "your-project",
  storageBucket: "your-project.appspot.com",
  messagingSenderId: "123456789",
  appId: "1:123456789:web:abc123",
};
```

### 3. ตั้งค่า Security Rules

#### สำหรับ Realtime Database

1. ไปที่ Firebase Console > Realtime Database > Rules
2. คัดลอกโค้ดจากไฟล์ `database.rules.json` ไปวาง
3. คลิก "Publish"

```json
{
  "rules": {
    "current_game": {
      ".read": true, // ทุกคนอ่านได้
      ".write": false // ห้ามเขียนจาก Frontend
    }
  }
}
```

#### สำหรับ Firestore

1. ไปที่ Firebase Console > Firestore Database > Rules
2. คัดลอกโค้ดจากไฟล์ `firestore.rules` ไปวาง
3. คลิก "Publish"

### 4. รันโปรเจกต์

```bash
npm run dev
```

เปิดเบราว์เซอร์ไปที่ `http://localhost:5173`

## การทดสอบ

### ทดสอบการอัปเดตแบบ Real-time

1. เปิดแอพในเบราว์เซอร์
2. เปิด Firebase Console > Realtime Database
3. แก้ไขค่า `score` เป็นตัวเลขอื่น
4. ดูหน้าเว็บ - ค่าควรเปลี่ยนทันที

### ทดสอบการจบเกม

1. ใน Firebase Console แก้ `status` จาก `"playing"` เป็น `"ended"`
2. หน้าเว็บควรแสดงฟอร์มให้กรอกชื่อ
3. กรอกชื่อและกด Submit
4. ไปเช็คที่ Firestore > leaderboard collection ว่าข้อมูลถูกบันทึกหรือไม่
5. เปลี่ยน status กลับเป็น `"playing"` เพื่อรีเซ็ต

## โครงสร้างของข้อมูล

### Realtime Database

```
/current_game
  ├── score: 0          (ตัวเลข - คะแนนปัจจุบัน)
  └── status: "playing" (ข้อความ - "playing" หรือ "ended")
```

### Firestore Collection: leaderboard

```javascript
{
  name: "ชื่อผู้เล่น",
  score: 100,
  createdAt: Timestamp
}
```

## หมายเหตุสำคัญ

- **Realtime Database**: Frontend อ่านได้อย่างเดียว ไม่สามารถเขียนได้ (ต้องแก้จาก Console หรือ Admin SDK)
- **Firestore**: สามารถส่งข้อมูลได้ แต่ห้ามแก้ไขหรือลบหลังจากส่งแล้ว
- ถ้าต้องการให้ Backend สามารถอัปเดต score ได้ ต้องใช้ Firebase Admin SDK หรือ Cloud Functions

## ปัญหาที่อาจพบ

### ❌ Permission Denied

- ตรวจสอบว่า Security Rules ถูกตั้งค่าถูกต้อง
- เช็ค Firebase Config ในไฟล์ `firebaseConfig.js`

### ❌ Score ไม่อัปเดต

- ตรวจสอบ databaseURL ใน config ว่าถูกต้อง
- ดูที่ Console ของเบราว์เซอร์ว่ามี error อะไร
- เช็คว่า node `/current_game` มีอยู่ใน Firebase

### ❌ ส่งฟอร์มไม่ได้

- ตรวจสอบ Firestore Rules
- ดู Console error ว่าขาด field ไหนไป

## การ Deploy

### Build

```bash
npm run build
```

### Deploy ไปที่ Firebase Hosting

```bash
npm install --save-dev firebase-tools
npx firebase login
npx firebase init
npx firebase deploy
```

## ติดต่อ

หากมีปัญหาหรือข้อสงสัย สามารถติดต่อได้ที่ทีมพัฒนา
