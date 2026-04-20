package com.example.helloapp_jeffreyadjei;

import android.content.Intent;
import android.database.Cursor;
import android.graphics.Typeface;
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
 * DashboardActivity manages the main user dashboard for the WeightMate application.
 *
 * Responsibilities:
 * - Displaying stored weight entries
 * - Adding new weight entries
 * - Deleting weight entries
 * - Setting and displaying the user's goal
 * - Navigating to the SMS permission screen
 *
 * CS 499 software engineering enhancements:
 * 1. Refactored large methods into smaller reusable methods
 * 2. Improved input validation and error handling
 * 3. Improved readability and maintainability
 * 4. Strengthened UI update flow
 * 5. Added clearer documentation and structured responsibilities
 */
public class DashboardActivity extends AppCompatActivity {

    private EditText inputDate;
    private EditText inputWeight;
    private EditText inputGoal;
    private Button addDataButton;
    private Button buttonOpenSmsPermission;
    private Button buttonSetGoal;
    private TableLayout dataGrid;
    private TextView textGoalDisplay;

    private DatabaseHelper db;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dashboard);

        initializeDatabase();
        initializeViews();
        setButtonListeners();
        refreshDashboard();
    }

    /**
     * Initializes the database helper.
     */
    private void initializeDatabase() {
        db = new DatabaseHelper(this);
    }

    /**
     * Connects Java fields to layout components.
     */
    private void initializeViews() {
        inputDate = findViewById(R.id.inputDate);
        inputWeight = findViewById(R.id.inputWeight);
        inputGoal = findViewById(R.id.inputGoal);
        buttonSetGoal = findViewById(R.id.buttonSetGoal);
        addDataButton = findViewById(R.id.addDataButton);
        buttonOpenSmsPermission = findViewById(R.id.buttonOpenSmsPermission);
        dataGrid = findViewById(R.id.dataGrid);
        textGoalDisplay = findViewById(R.id.textGoalDisplay);
    }

    /**
     * Assigns event listeners to dashboard buttons.
     */
    private void setButtonListeners() {
        addDataButton.setOnClickListener(v -> onAddEntry());
        buttonSetGoal.setOnClickListener(v -> onSetGoal());
        buttonOpenSmsPermission.setOnClickListener(v -> openSmsPermissionScreen());
    }

    /**
     * Refreshes all dashboard UI components.
     */
    private void refreshDashboard() {
        refreshGoalLabel();
        buildTable();
    }

    /**
     * Opens the SMS permission activity.
     */
    private void openSmsPermissionScreen() {
        Intent intent = new Intent(this, SMSPermissionActivity.class);
        startActivity(intent);
    }

    /**
     * Handles adding a new weight entry after validation.
     */
    private void onAddEntry() {
        String date = getTrimmedText(inputDate);
        String weightText = getTrimmedText(inputWeight);

        if (!isWeightInputValid(date, weightText)) {
            return;
        }

        double weightValue;
        try {
            weightValue = Double.parseDouble(weightText);
        } catch (NumberFormatException e) {
            inputWeight.setError("Please enter a valid numeric weight");
            inputWeight.requestFocus();
            return;
        }

        long rowId = db.insertWeight(date, weightValue);

        if (rowId > 0) {
            clearWeightInputFields();
            buildTable();
            showToast("Weight entry added successfully");
        } else {
            showToast("Failed to add entry");
        }
    }

    /**
     * Handles updating the user's goal value.
     */
    private void onSetGoal() {
        String goalText = getTrimmedText(inputGoal);

        if (TextUtils.isEmpty(goalText)) {
            inputGoal.setError("Goal is required");
            inputGoal.requestFocus();
            return;
        }

        double goalValue;
        try {
            goalValue = Double.parseDouble(goalText);
        } catch (NumberFormatException e) {
            inputGoal.setError("Please enter a valid numeric goal");
            inputGoal.requestFocus();
            return;
        }

        if (goalValue <= 0 || goalValue >= 1400) {
            inputGoal.setError("Please enter a realistic goal");
            inputGoal.requestFocus();
            return;
        }

        db.setGoal(goalValue);
        inputGoal.setText("");
        refreshGoalLabel();
        showToast("Goal updated successfully");
    }

    /**
     * Updates the goal label on screen.
     */
    private void refreshGoalLabel() {
        double currentGoal = db.getGoal();
        textGoalDisplay.setText("Goal: " + currentGoal);
    }

    /**
     * Rebuilds the data table from current database entries.
     */
    private void buildTable() {
        dataGrid.removeAllViews();
        addTableHeader();

        Cursor cursor = null;
        try {
            cursor = db.getAllWeights();

            if (cursor != null) {
                while (cursor.moveToNext()) {
                    long id = cursor.getLong(0);
                    String date = cursor.getString(1);
                    double weight = cursor.getDouble(2);
                    addDataRow(id, date, weight);
                }
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }

    /**
     * Adds the header row to the table.
     */
    private void addTableHeader() {
        TableRow header = new TableRow(this);
        addCell(header, "Date", true);
        addCell(header, "Weight", true);
        addCell(header, "Action", true);
        dataGrid.addView(header);
    }

    /**
     * Adds a data row for a single weight record.
     *
     * @param id database record ID
     * @param date date string
     * @param weight weight value
     */
    private void addDataRow(long id, String date, double weight) {
        TableRow row = new TableRow(this);

        addCell(row, date, false);
        addCell(row, String.valueOf(weight), false);

        TextView deleteButton = new TextView(this);
        deleteButton.setText("Delete");
        deleteButton.setPadding(24, 16, 24, 16);
        deleteButton.setTypeface(deleteButton.getTypeface(), Typeface.BOLD);

        deleteButton.setOnClickListener(v -> {
            int rowsDeleted = db.deleteWeight(id);
            if (rowsDeleted > 0) {
                buildTable();
                showToast("Entry deleted");
            } else {
                showToast("Unable to delete entry");
            }
        });

        row.addView(deleteButton);
        dataGrid.addView(row);
    }

    /**
     * Adds a single cell to the given row.
     *
     * @param row target row
     * @param text displayed text
     * @param bold whether text should be bold
     */
    private void addCell(TableRow row, String text, boolean bold) {
        TextView textView = new TextView(this);
        textView.setText(text);
        textView.setPadding(24, 16, 24, 16);

        if (bold) {
            textView.setTypeface(textView.getTypeface(), Typeface.BOLD);
        }

        row.addView(textView);
    }

    /**
     * Returns trimmed text from an EditText.
     *
     * @param editText source EditText
     * @return trimmed string
     */
    private String getTrimmedText(EditText editText) {
        return editText.getText().toString().trim();
    }

    /**
     * Validates date and weight input values.
     *
     * @param date date string
     * @param weightText weight string
     * @return true if valid; false otherwise
     */
    private boolean isWeightInputValid(String date, String weightText) {
        if (TextUtils.isEmpty(date)) {
            inputDate.setError("Date is required");
            inputDate.requestFocus();
            return false;
        }

        if (TextUtils.isEmpty(weightText)) {
            inputWeight.setError("Weight is required");
            inputWeight.requestFocus();
            return false;
        }

        return true;
    }

    /**
     * Clears the date and weight input fields.
     */
    private void clearWeightInputFields() {
        inputDate.setText("");
        inputWeight.setText("");
    }

    /**
     * Displays a short toast message.
     *
     * @param message text to display
     */
    private void showToast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }
}