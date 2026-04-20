package com.example.helloapp_jeffreyadjei;

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.text.TextUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

/**
 * Dashboard: shows weight grid with Add/Delete and goal setter, plus SMS Permission screen link.
 */
public class DashboardActivity extends AppCompatActivity {

    private EditText inputDate, inputWeight, inputGoal;
    private Button addDataButton, buttonOpenSmsPermission, buttonSetGoal;
    private TableLayout dataGrid;
    private TextView textGoalDisplay;

    private DatabaseHelper db;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dashboard);

        db = new DatabaseHelper(this);

        inputDate = findViewById(R.id.inputDate);
        inputWeight = findViewById(R.id.inputWeight);
        inputGoal = findViewById(R.id.inputGoal);
        buttonSetGoal = findViewById(R.id.buttonSetGoal);
        addDataButton = findViewById(R.id.addDataButton);
        buttonOpenSmsPermission = findViewById(R.id.buttonOpenSmsPermission);
        dataGrid = findViewById(R.id.dataGrid);
        textGoalDisplay = findViewById(R.id.textGoalDisplay);

        refreshGoalLabel();
        buildTable();

        addDataButton.setOnClickListener(v -> onAddEntry());
        buttonOpenSmsPermission.setOnClickListener(v ->
                startActivity(new Intent(this, SMSPermissionActivity.class)));
        buttonSetGoal.setOnClickListener(v -> onSetGoal());
    }

    private void onAddEntry() {
        String date = inputDate.getText().toString().trim();
        String weightStr = inputWeight.getText().toString().trim();

        if (TextUtils.isEmpty(date) || TextUtils.isEmpty(weightStr)) {
            Toast.makeText(this, "Please fill in date and weight", Toast.LENGTH_SHORT).show();
            return;
        }

        double weight;
        try {
            weight = Double.parseDouble(weightStr);
        } catch (NumberFormatException e) {
            Toast.makeText(this, "Invalid weight", Toast.LENGTH_SHORT).show();
            return;
        }

        long rowId = db.insertWeight(date, weight);
        if (rowId > 0) {
            inputDate.setText("");
            inputWeight.setText("");
            buildTable();
        } else {
            Toast.makeText(this, "Failed to add entry", Toast.LENGTH_SHORT).show();
        }
    }

    private void onSetGoal() {
        String g = inputGoal.getText().toString().trim();
        if (TextUtils.isEmpty(g)) {
            Toast.makeText(this, "Enter a goal", Toast.LENGTH_SHORT).show();
            return;
        }
        try {
            db.setGoal(Double.parseDouble(g));
            inputGoal.setText("");
            refreshGoalLabel();
            Toast.makeText(this, "Goal updated", Toast.LENGTH_SHORT).show();
        } catch (NumberFormatException e) {
            Toast.makeText(this, "Invalid goal", Toast.LENGTH_SHORT).show();
        }
    }

    private void refreshGoalLabel() {
        textGoalDisplay.setText("Goal: " + db.getGoal());
    }

    private void buildTable() {
        dataGrid.removeAllViews();

        // header
        TableRow header = new TableRow(this);
        addCell(header, "Date", true);
        addCell(header, "Weight", true);
        addCell(header, "Action", true);
        dataGrid.addView(header);

        Cursor c = db.getAllWeights();
        while (c.moveToNext()) {
            long id = c.getLong(0);
            String date = c.getString(1);
            double weight = c.getDouble(2);

            TableRow row = new TableRow(this);
            addCell(row, date, false);
            addCell(row, String.valueOf(weight), false);

            TextView deleteBtn = new TextView(this);
            deleteBtn.setText("🗑️ Delete");
            deleteBtn.setPadding(24, 16, 24, 16);
            deleteBtn.setOnClickListener(v -> {
                db.deleteWeight(id);
                buildTable();
            });
            row.addView(deleteBtn);

            dataGrid.addView(row);
        }
        c.close();
    }

    private void addCell(TableRow row, String text, boolean bold) {
        TextView tv = new TextView(this);
        tv.setText(text);
        tv.setPadding(24, 16, 24, 16);
        if (bold) tv.setTypeface(tv.getTypeface(), android.graphics.Typeface.BOLD);
        row.addView(tv);
    }
}