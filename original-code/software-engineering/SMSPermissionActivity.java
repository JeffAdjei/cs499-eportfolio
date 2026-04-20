package com.example.helloapp_jeffreyadjei;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class SMSPermissionActivity extends AppCompatActivity {

    private static final int REQUEST_SMS_PERMISSION = 123;

    private TextView textPermissionStatus;
    private Button buttonGrantPermission;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sms_permission);

        textPermissionStatus = findViewById(R.id.textPermissionStatus);
        buttonGrantPermission = findViewById(R.id.buttonGrantPermission);

        // Check current permission status
        updatePermissionStatus();

        buttonGrantPermission.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (ContextCompat.checkSelfPermission(SMSPermissionActivity.this,
                        Manifest.permission.SEND_SMS) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(SMSPermissionActivity.this,
                            new String[]{Manifest.permission.SEND_SMS},
                            REQUEST_SMS_PERMISSION);
                } else {
                    textPermissionStatus.setText("Permission already granted.");
                }
            }
        });
    }

    private void updatePermissionStatus() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.SEND_SMS) == PackageManager.PERMISSION_GRANTED) {
            textPermissionStatus.setText("SMS permission granted.");
        } else {
            textPermissionStatus.setText("SMS permission NOT granted.");
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == REQUEST_SMS_PERMISSION) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                textPermissionStatus.setText("Permission granted. You will now receive notifications.");
            } else {
                textPermissionStatus.setText("Permission denied. App will still function but without notifications.");
            }
        }
    }
}