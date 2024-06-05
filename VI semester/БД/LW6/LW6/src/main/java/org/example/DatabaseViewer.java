package org.example;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableModel;
import javax.swing.table.TableRowSorter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.sql.*;
import java.util.Vector;

public class DatabaseViewer extends JFrame {
    private JComboBox<String> tables_list;
    private JTable table_db;
    private JScrollPane scroll_panel;
    private JButton insert_button;
    private JTextField[] input_fields;
    private Connection connection;

    public DatabaseViewer() {
        setTitle("Travel agency");
        setSize(600, 400);
        setLayout(new BorderLayout());

        setDefaultCloseOperation(EXIT_ON_CLOSE);

        JMenuBar menu_bar = new JMenuBar();
        setJMenuBar(menu_bar);

        JMenu file_menu = new JMenu("File");
        menu_bar.add(file_menu);

        JMenuItem add_item = new JMenuItem("Add data...");
        add_item.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                addNewRow();
            }
        });
        file_menu.add(add_item);

        JMenuItem delete_item = new JMenuItem("Delete data");
        delete_item.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                deleteSelectedRow();
            }
        });
        file_menu.add(delete_item);

        tables_list = new JComboBox<>();
        tables_list.addActionListener(e -> {
            String selected = (String) tables_list.getSelectedItem();
            if (selected != null)
                displayTable(selected);
        });
        add(tables_list, BorderLayout.NORTH);

        table_db = new JTable();
        table_db.setAutoCreateRowSorter(true);
        table_db.getTableHeader().addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                int column = table_db.columnAtPoint(e.getPoint());
                if (column != -1) {
                    String table_name = (String) tables_list.getSelectedItem();
                    String column_name = table_db.getColumnName(column);
                    displayTable(table_name, column_name);
                }
            }
        });

        scroll_panel = new JScrollPane(table_db);
        add(scroll_panel, BorderLayout.CENTER);

        try {
            connectToDatabase();
            loadTables();
        }
        catch (SQLException e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(this, "Failed to connect to database: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void connectToDatabase() throws SQLException {
        String url = "jdbc:postgresql://localhost:5432/Travel+agency?user=nina";
        connection = DriverManager.getConnection(url);
    }

    private void loadTables() throws SQLException {
        DatabaseMetaData data_db = connection.getMetaData();
        ResultSet tables = data_db.getTables(null, null, null, new String[]{"TABLE"});

        while (tables.next()) {
            String table_name = tables.getString("TABLE_NAME");
            tables_list.addItem(table_name);
        }
    }

    private void displayTable(String table_name) {
        displayTable(table_name, null);
    }

    private void displayTable(String table_name, String order_by_column) {
        try {
            String query = "SELECT * FROM " + table_name;
            if (order_by_column != null) {
                query += " ORDER BY " + order_by_column;
            }

            Statement statement = connection.createStatement();
            ResultSet result = statement.executeQuery(query);
            ResultSetMetaData meta_data = result.getMetaData();

            int col_counter = meta_data.getColumnCount();
            Vector<String> columns = new Vector<>();
            Vector<Vector<Object>> data = new Vector<>();

            for (int i=1; i<=col_counter; i++)
                columns.add(meta_data.getColumnName(i));

            while (result.next()) {
                Vector<Object> row = new Vector<>();
                for (int i=1; i<=col_counter; i++)
                    row.add(result.getObject(i));
                data.add(row);
            }

            DefaultTableModel model = new DefaultTableModel(data, columns);
            table_db.setModel(model);
        }
        catch (SQLException e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(this, "Failed to display table: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void addNewRow() {
        try {
            String table_name = (String) tables_list.getSelectedItem();
            if (table_name == null) {
                JOptionPane.showMessageDialog(this, "Choose a table for adding new data", "Error", JOptionPane.ERROR_MESSAGE);
                return;
            }

            Statement statement = connection.createStatement();

            ResultSet result = statement.executeQuery("SELECT * FROM " + table_name);
            ResultSetMetaData meta_data = result.getMetaData();

            int col_counter = meta_data.getColumnCount();
            String[] columns = new String[col_counter];
            input_fields = new JTextField[col_counter];

            JPanel input_panel = new JPanel(new GridLayout(col_counter, 2));

            for (int i=1; i<=col_counter; i++) {
                columns[i-1] = meta_data.getColumnName(i);
                input_panel.add(new JLabel(columns[i-1]));

                JTextField text_field = new JTextField();
                input_fields[i-1] = text_field;
                input_panel.add(text_field);
            }

            int insertion = JOptionPane.showConfirmDialog(null, input_panel, "Enter data", JOptionPane.OK_CANCEL_OPTION);
            if (insertion == JOptionPane.OK_OPTION) {
                StringBuilder query = new StringBuilder("INSERT INTO " + table_name + " (");

                for (int i=0; i<col_counter; i++) {
                    query.append(columns[i]);
                    if (i < col_counter - 1)
                        query.append(", ");
                }

                query.append(") VALUES (");
                for (int i=0; i<col_counter; i++) {
                    query.append("'" + input_fields[i].getText() + "'");
                    if (i < col_counter - 1)
                        query.append(", ");
                }
                query.append(")");

                connection.createStatement().executeUpdate(query.toString());
                displayTable(table_name);
            }
        }
        catch (SQLException e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(this, "Failed to add a new row: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void deleteSelectedRow() {
        try {
            String table_name = (String) tables_list.getSelectedItem();
            if (table_name == null) {
                JOptionPane.showMessageDialog(this, "Choose a table for deleting data", "Error", JOptionPane.ERROR_MESSAGE);
                return;
            }

            int selected_row = table_db.getSelectedRow();
            if (selected_row == -1) {
                JOptionPane.showMessageDialog(this, "Choose, which row to delete", "Error", JOptionPane.ERROR_MESSAGE);
                return;
            }

            TableModel model = table_db.getModel();
            String key_column = model.getColumnName(0);
            Object id = table_db.getValueAt(selected_row, 0);

            String query;
            if(table_name.equals("clients") || table_name.equals("managers"))
                query = "DELETE FROM " + table_name + " WHERE " + key_column + " = '" + id + "'";
            else
                query = "DELETE FROM " + table_name + " WHERE " + key_column + " = " + id;

            connection.createStatement().executeUpdate(query);
            displayTable(table_name);
        }
        catch (SQLException e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(this, "Failed to delete a row: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            DatabaseViewer viewer = new DatabaseViewer();
            viewer.setVisible(true);
        });
    }
}