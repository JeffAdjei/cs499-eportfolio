package com.example.helloapp_jeffreyadjei;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

/**
 * SMSPermissionActivity handles runtime permission requests for sending SMS.
 *
 * Enhancements for CS 499:
 * 1. Improved structure and readability
 * 2. Better user feedback messaging
 * 3. Reusable helper methods
 * 4. Defensive permission handling
 */
public class SMSPermissionActivity extends AppCompatActivity {

    private static final int REQUEST_SMS_PERMISSION = 123;

    private TextView textPermissionStatus;
    private Button buttonGrantPermission;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sms_permission);

        initializeViews();
        updatePermissionStatus();
        setListeners();
    }

    /**
     * Initializes UI components.
     */
    private void initializeViews() {
        textPermissionStatus = findViewById(R.id.textPermissionStatus);
        buttonGrantPermission = findViewById(R.id.buttonGrantPermission);
    }

    /**
     * Assigns click listener to the permission button.
     */
    private void setListeners() {
        buttonGrantPermission.setOnClickListener(v -> requestSmsPermission());
    }

    /**
     * Requests SMS permission if not already granted.
     */
    private void requestSmsPermission() {
        if (!isPermissionGranted()) {
            ActivityCompat.requestPermissions(
                    this,
                    new String[]{Manifest.permission.SEND_SMS},
                    REQUEST_SMS_PERMISSION
            );
        } else {
            textPermissionStatus.setText("Permission already granted.");
        }
    }

    /**
     * Checks if SMS permission is granted.
     */
    private boolean isPermissionGranted() {
        return ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.SEND_SMS
        ) == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * Updates UI to reflect current permission status.
     */
    private void updatePermissionStatus() {
        if (isPermissionGranted()) {
            textPermissionStatus.setText("SMS permission is currently granted.");
        } else {
            textPermissionStatus.setText("SMS permission is not granted.");
        }
    }

    /**
     * Handles permission request result.
     */
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {

        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == REQUEST_SMS_PERMISSION) {
            handlePermissionResult(grantResults);
        }
    }

    /**
     * Processes the permission result.
     */
    private void handlePermissionResult(int[] grantResults) {
        if (grantResults.length > 0 &&
                grantResults[0] == PackageManager.PERMISSION_GRANTED) {

            textPermissionStatus.setText(
                    "Permission granted. SMS features are now enabled."
            );

        } else {
            textPermissionStatus.setText(
                    "Permission denied. SMS features will remain disabled."
            );
        }
    }
}