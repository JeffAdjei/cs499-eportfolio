package com.example.helloapp_jeffreyadjei;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

/**
 * MainActivity serves as the login and account creation entry point
 * for the Weight Tracking application.
 *
 * Enhancements made for CS 499 software design and engineering milestone:
 * 1. Improved code organization through helper methods
 * 2. Stronger input validation
 * 3. Reusable event-handling logic
 * 4. Clearer documentation and maintainability
 * 5. Better user flow after successful login/account creation
 */
public class MainActivity extends AppCompatActivity {

    private EditText editTextUsername;
    private EditText editTextPassword;
    private Button buttonLogin;
    private Button buttonCreateAccount;

    private DatabaseHelper db;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        initializeDatabase();
        initializeViews();
        setButtonListeners();
    }

    /**
     * Initializes the database helper used for authentication and registration.
     */
    private void initializeDatabase() {
        db = new DatabaseHelper(this);
    }

    /**
     * Links Java variables to UI components in the layout.
     */
    private void initializeViews() {
        editTextUsername = findViewById(R.id.editTextUsername);
        editTextPassword = findViewById(R.id.editTextPassword);
        buttonLogin = findViewById(R.id.buttonLogin);
        buttonCreateAccount = findViewById(R.id.buttonCreateAccount);
    }

    /**
     * Assigns click listeners to the login and create-account buttons.
     */
    private void setButtonListeners() {
        buttonLogin.setOnClickListener(v -> handleLogin());
        buttonCreateAccount.setOnClickListener(v -> handleCreateAccount());
    }

    /**
     * Handles the login process after validating user input.
     */
    private void handleLogin() {
        String username = getTrimmedUsername();
        String password = getTrimmedPassword();

        if (!isInputValid(username, password)) {
            return;
        }

        boolean isValidUser = db.validateUser(username, password);

        if (isValidUser) {
            showToast("Login successful");
            navigateToDashboard();
        } else {
            showToast("Invalid username or password");
        }
    }

    /**
     * Handles account creation after validating user input.
     */
    private void handleCreateAccount() {
        String username = getTrimmedUsername();
        String password = getTrimmedPassword();

        if (!isInputValid(username, password)) {
            return;
        }

        long userId = db.registerUser(username, password);

        if (userId > 0) {
            showToast("Account created successfully");
            clearInputFields();
        } else {
            showToast("Account already exists");
        }
    }

    /**
     * Retrieves and trims the username entered by the user.
     *
     * @return trimmed username string
     */
    private String getTrimmedUsername() {
        return editTextUsername.getText().toString().trim();
    }

    /**
     * Retrieves and trims the password entered by the user.
     *
     * @return trimmed password string
     */
    private String getTrimmedPassword() {
        return editTextPassword.getText().toString().trim();
    }

    /**
     * Validates username and password input fields.
     *
     * @param username entered username
     * @param password entered password
     * @return true if valid, false otherwise
     */
    private boolean isInputValid(String username, String password) {
        if (TextUtils.isEmpty(username)) {
            editTextUsername.setError("Username is required");
            editTextUsername.requestFocus();
            return false;
        }

        if (TextUtils.isEmpty(password)) {
            editTextPassword.setError("Password is required");
            editTextPassword.requestFocus();
            return false;
        }

        if (username.length() < 3) {
            editTextUsername.setError("Username must be at least 3 characters");
            editTextUsername.requestFocus();
            return false;
        }

        if (password.length() < 4) {
            editTextPassword.setError("Password must be at least 4 characters");
            editTextPassword.requestFocus();
            return false;
        }

        return true;
    }

    /**
     * Navigates the user to the dashboard screen after successful login.
     */
    private void navigateToDashboard() {
        Intent intent = new Intent(MainActivity.this, DashboardActivity.class);
        startActivity(intent);
        finish();
    }

    /**
     * Clears the username and password fields after successful account creation.
     */
    private void clearInputFields() {
        editTextUsername.setText("");
        editTextPassword.setText("");
    }

    /**
     * Displays a short toast message to the user.
     *
     * @param message text to display
     */
    private void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }
}