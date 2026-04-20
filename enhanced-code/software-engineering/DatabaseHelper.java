package com.example.helloapp_jeffreyadjei;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * DatabaseHelper manages all SQLite operations for the WeightMate application.
 *
 * Enhancements for CS 499:
 * 1. Centralized schema constants for maintainability
 * 2. Added defensive input validation
 * 3. Improved code readability and modularity
 * 4. Added password hashing for better security practice
 * 5. Added safer cursor handling and reusable helper methods
 */
public class DatabaseHelper extends SQLiteOpenHelper {

    private static final String DB_NAME = "weightmate.db";
    private static final int DB_VERSION = 1;

    // Table names
    public static final String T_USERS = "users";
    public static final String T_WEIGHTS = "weights";
    public static final String T_GOAL = "goal";

    // Common column names
    public static final String COL_ID = "id";

    // Users table columns
    public static final String COL_USERNAME = "username";
    public static final String COL_PASSWORD = "password";

    // Weights table columns
    public static final String COL_DATE = "date";
    public static final String COL_WEIGHT = "weight";

    // Goal table columns
    public static final String COL_GOAL = "goal";

    public DatabaseHelper(Context ctx) {
        super(ctx, DB_NAME, null, DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        createUsersTable(db);
        createWeightsTable(db);
        createGoalTable(db);
        seedDefaultGoal(db);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + T_USERS);
        db.execSQL("DROP TABLE IF EXISTS " + T_WEIGHTS);
        db.execSQL("DROP TABLE IF EXISTS " + T_GOAL);
        onCreate(db);
    }

    /**
     * Creates the users table.
     */
    private void createUsersTable(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE " + T_USERS + " (" +
                COL_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                COL_USERNAME + " TEXT UNIQUE NOT NULL, " +
                COL_PASSWORD + " TEXT NOT NULL)");
    }

    /**
     * Creates the weights table.
     */
    private void createWeightsTable(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE " + T_WEIGHTS + " (" +
                COL_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                COL_DATE + " TEXT NOT NULL, " +
                COL_WEIGHT + " REAL NOT NULL)");
    }

    /**
     * Creates the goal table.
     */
    private void createGoalTable(SQLiteDatabase db) {
        db.execSQL("CREATE TABLE " + T_GOAL + " (" +
                COL_ID + " INTEGER PRIMARY KEY CHECK(" + COL_ID + "=1), " +
                COL_GOAL + " REAL NOT NULL)");
    }

    /**
     * Seeds a default goal row so that the application always has one goal record.
     */
    private void seedDefaultGoal(SQLiteDatabase db) {
        ContentValues cv = new ContentValues();
        cv.put(COL_ID, 1);
        cv.put(COL_GOAL, 0.0);
        db.insert(T_GOAL, null, cv);
    }

    // ---------------------------------------------------
    // User Authentication Methods
    // ---------------------------------------------------

    /**
     * Validates whether a username and password match a stored user record.
     *
     * @param username the username entered
     * @param password the password entered
     * @return true if valid; false otherwise
     */
    public boolean validateUser(String username, String password) {
        if (!isValidCredentialInput(username, password)) {
            return false;
        }

        SQLiteDatabase db = getReadableDatabase();
        Cursor cursor = null;
        boolean isValid = false;

        try {
            String hashedPassword = hashPassword(password);

            cursor = db.query(
                    T_USERS,
                    new String[]{COL_ID},
                    COL_USERNAME + "=? AND " + COL_PASSWORD + "=?",
                    new String[]{username.trim(), hashedPassword},
                    null,
                    null,
                    null
            );

            isValid = cursor != null && cursor.moveToFirst();
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        return isValid;
    }

    /**
     * Registers a new user.
     *
     * @param username the username to register
     * @param password the password to register
     * @return row ID if successful, -1 if failed
     */
    public long registerUser(String username, String password) {
        if (!isValidCredentialInput(username, password)) {
            return -1;
        }

        SQLiteDatabase db = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put(COL_USERNAME, username.trim());
        cv.put(COL_PASSWORD, hashPassword(password));

        return db.insert(T_USERS, null, cv);
    }

    /**
     * Validates basic credential input.
     */
    private boolean isValidCredentialInput(String username, String password) {
        return username != null
                && password != null
                && username.trim().length() >= 3
                && password.trim().length() >= 4;
    }

    // ---------------------------------------------------
    // Weight CRUD Methods
    // ---------------------------------------------------

    /**
     * Inserts a new weight record.
     *
     * @param date the date of the weight entry
     * @param weight the weight value
     * @return row ID if successful, -1 if failed
     */
    public long insertWeight(String date, double weight) {
        if (!isValidWeightEntry(date, weight)) {
            return -1;
        }

        SQLiteDatabase db = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put(COL_DATE, date.trim());
        cv.put(COL_WEIGHT, weight);

        return db.insert(T_WEIGHTS, null, cv);
    }

    /**
     * Deletes a weight record by ID.
     *
     * @param id record ID
     * @return number of rows deleted
     */
    public int deleteWeight(long id) {
        SQLiteDatabase db = getWritableDatabase();
        return db.delete(T_WEIGHTS, COL_ID + "=?", new String[]{String.valueOf(id)});
    }

    /**
     * Updates an existing weight record.
     *
     * @param id record ID
     * @param date updated date
     * @param weight updated weight
     * @return number of rows updated
     */
    public int updateWeight(long id, String date, double weight) {
        if (!isValidWeightEntry(date, weight)) {
            return 0;
        }

        SQLiteDatabase db = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put(COL_DATE, date.trim());
        cv.put(COL_WEIGHT, weight);

        return db.update(T_WEIGHTS, cv, COL_ID + "=?", new String[]{String.valueOf(id)});
    }

    /**
     * Retrieves all weight entries in descending order.
     *
     * @return cursor containing weight records
     */
    public Cursor getAllWeights() {
        SQLiteDatabase db = getReadableDatabase();
        return db.query(
                T_WEIGHTS,
                new String[]{COL_ID, COL_DATE, COL_WEIGHT},
                null,
                null,
                null,
                null,
                COL_ID + " DESC"
        );
    }

    /**
     * Validates date and weight values.
     */
    private boolean isValidWeightEntry(String date, double weight) {
        return date != null
                && !date.trim().isEmpty()
                && weight > 0
                && weight < 1400;
    }

    // ---------------------------------------------------
    // Goal Methods
    // ---------------------------------------------------

    /**
     * Updates the user's goal value.
     *
     * @param goal goal weight
     */
    public void setGoal(double goal) {
        if (goal <= 0 || goal >= 1400) {
            return;
        }

        SQLiteDatabase db = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put(COL_GOAL, goal);
        db.update(T_GOAL, cv, COL_ID + "=1", null);
    }

    /**
     * Retrieves the stored goal value.
     *
     * @return goal weight, or 0.0 if no goal is found
     */
    public double getGoal() {
        SQLiteDatabase db = getReadableDatabase();
        Cursor cursor = null;
        double goalValue = 0.0;

        try {
            cursor = db.query(
                    T_GOAL,
                    new String[]{COL_GOAL},
                    COL_ID + "=?",
                    new String[]{"1"},
                    null,
                    null,
                    null
            );

            if (cursor != null && cursor.moveToFirst()) {
                goalValue = cursor.getDouble(0);
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        return goalValue;
    }

    // ---------------------------------------------------
    // Security Utility
    // ---------------------------------------------------

    /**
     * Hashes a password using SHA-256 before storing or validating.
     *
     * @param password raw password
     * @return hashed password
     */
    private String hashPassword(String password) {
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] hashedBytes = digest.digest(password.getBytes());
            StringBuilder builder = new StringBuilder();

            for (byte b : hashedBytes) {
                builder.append(String.format("%02x", b));
            }

            return builder.toString();
        } catch (NoSuchAlgorithmException e) {
            // Fallback to raw password only if hashing algorithm is unavailable
            return password;
        }
    }
}