package com.example.helloapp_jeffreyadjei;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

/**
 * SQLite for WeightMate (Project 3)
 * Tables:
 *  - users(id, username UNIQUE, password)
 *  - weights(id, date TEXT, weight REAL)
 *  - goal(id=1 single row, goal REAL)
 */
public class DatabaseHelper extends SQLiteOpenHelper {

    private static final String DB_NAME = "weightmate.db";
    private static final int DB_VERSION = 1;

    public static final String T_USERS   = "users";
    public static final String T_WEIGHTS = "weights";
    public static final String T_GOAL    = "goal";

    public DatabaseHelper(Context ctx) {
        super(ctx, DB_NAME, null, DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE " + T_USERS + " (" +
                "id INTEGER PRIMARY KEY AUTOINCREMENT," +
                "username TEXT UNIQUE," +
                "password TEXT)");

        db.execSQL("CREATE TABLE " + T_WEIGHTS + " (" +
                "id INTEGER PRIMARY KEY AUTOINCREMENT," +
                "date TEXT," +
                "weight REAL)");

        db.execSQL("CREATE TABLE " + T_GOAL + " (" +
                "id INTEGER PRIMARY KEY CHECK(id=1)," +
                "goal REAL)");

        // seed goal row
        ContentValues cv = new ContentValues();
        cv.put("id", 1);
        cv.put("goal", 0.0);
        db.insert(T_GOAL, null, cv);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + T_USERS);
        db.execSQL("DROP TABLE IF EXISTS " + T_WEIGHTS);
        db.execSQL("DROP TABLE IF EXISTS " + T_GOAL);
        onCreate(db);
    }

    // ---- Users ----
    public boolean validateUser(String username, String password) {
        SQLiteDatabase db = getReadableDatabase();
        Cursor c = db.rawQuery("SELECT id FROM " + T_USERS + " WHERE username=? AND password=?",
                new String[]{username, password});
        boolean ok = c.moveToFirst();
        c.close();
        return ok;
    }

    public long registerUser(String username, String password) {
        // returns -1 if insert fails (e.g., duplicate username)
        SQLiteDatabase db = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put("username", username);
        cv.put("password", password);
        return db.insert(T_USERS, null, cv);
    }

    // ---- Weights CRUD ----
    public long insertWeight(String date, double weight) {
        SQLiteDatabase db = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put("date", date);
        cv.put("weight", weight);
        return db.insert(T_WEIGHTS, null, cv);
    }

    public int deleteWeight(long id) {
        SQLiteDatabase db = getWritableDatabase();
        return db.delete(T_WEIGHTS, "id=?", new String[]{String.valueOf(id)});
    }

    public int updateWeight(long id, String date, double weight) {
        SQLiteDatabase db = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put("date", date);
        cv.put("weight", weight);
        return db.update(T_WEIGHTS, cv, "id=?", new String[]{String.valueOf(id)});
    }

    public Cursor getAllWeights() {
        SQLiteDatabase db = getReadableDatabase();
        return db.rawQuery("SELECT id, date, weight FROM " + T_WEIGHTS + " ORDER BY id DESC", null);
    }

    // ---- Goal ----
    public void setGoal(double goal) {
        SQLiteDatabase db = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put("goal", goal);
        db.update(T_GOAL, cv, "id=1", null);
    }

    public double getGoal() {
        SQLiteDatabase db = getReadableDatabase();
        Cursor c = db.rawQuery("SELECT goal FROM " + T_GOAL + " WHERE id=1", null);
        double g = 0.0;
        if (c.moveToFirst()) g = c.getDouble(0);
        c.close();
        return g;
    }
}