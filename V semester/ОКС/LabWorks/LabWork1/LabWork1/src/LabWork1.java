import javax.swing.*;
import javax.swing.Timer;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.*;
import java.awt.event.*;
import java.io.UnsupportedEncodingException;
import java.util.*;
import java.util.List;

import com.fazecast.jSerialComm.*;

public class LabWork1 {
    static int bytes_received_num = 0;
    private static String port_name = "COM1";
    private static SerialPort com_port;
    private static int selected_port_num = 1;
    private static int selected_stopbits = 1;
    private static int flag = 0;
    private static JTextArea input_area;
    private static JTextArea output_area;
    private static JComboBox<Integer> com_port_number;
    static JLabel label_frame;
    static JLabel label_total;
    static JTextArea debug_area;
    static final Integer DATA_LENGTH = 1;
    private static Integer data_counter = 0;

    /**
     * Метод для настройки COM-порта по умолчанию.
     */
    public static void setUpPort() {
        com_port = SerialPort.getCommPort(port_name);
        com_port.setBaudRate(115200);
        com_port.setNumStopBits(1);
        com_port.setParity(SerialPort.NO_PARITY);
    }

    /**
     * Метод для формирования списка доступных COM-портов.
     *
     * @return Список номеров доступных COM-портов
     */
    public static Integer[] getListOfAvailablePorts() {
        SerialPort[] ports_available = SerialPort.getCommPorts();
        ArrayList<Integer> port_numbers = new ArrayList<>();

        for (SerialPort serialPort : ports_available) {
            String name = serialPort.getSystemPortName();

            int number = Integer.parseInt(name.replaceAll("\\D", ""));
            if (number < 10)    //чтобы обеспечить возможность вхождения номера порта в структуру кадра
                port_numbers.add(number);
        }

        Collections.sort(port_numbers);

        return port_numbers.toArray(new Integer[0]);
    }

    /**
     * Метод для открытия COM-порта по умолчанию.
     *
     * @param label_message текстовое поле для обновления информации при открытии порта
     */
    public static void openPortByDefault(JLabel label_message) {
        if (!com_port.isOpen()) {
            com_port.openPort();
            if (!PortAccessManager.tryOpenPort(port_name)) {
                debug_area.append("COM1-port can't be opened or has already been opened\n");
                label_message.setText("  COM2 and 1 stopbit by default");

                com_port_number.setSelectedItem(2);

                port_name = "COM2";
                selected_port_num = 2;
                com_port = SerialPort.getCommPort(port_name);
                com_port.openPort();
                PortAccessManager.tryOpenPort("COM" + selected_port_num);
            }
            debug_area.append(port_name + " has been opened\n");
        } else debug_area.append("COM-port can't be opened or has already been opened\n");
    }

    /**
     * Метод для поясняющего вывода в окно состояния кадра после байт-стаффинга перед отправкой.
     *
     * @param frame_encoded кадр после кодирования и байт-стаффинга
     */
    private static void printFrame(byte[] frame_encoded) {
        StringBuilder frame_displayed;

        try {
            frame_displayed = new StringBuilder(new String(Arrays.copyOfRange(frame_encoded, 0, frame_encoded.length - 1), "CP1251"));
        } catch (UnsupportedEncodingException ex) {
            throw new RuntimeException(ex);
        }
        if (frame_encoded[4] == 10)    //если встречен Enter
            frame_displayed.replace(4, 5, "\\n");

        frame_displayed.append(String.format(" (%d)", frame_encoded[frame_encoded.length - 1]));    //FCS - в 10-чном виде

        label_frame.setText("         Frame: " + frame_displayed);
    }

    /**
     * Метод для проверки валидности полученного кадра (в частности, не произошла ли склейка).
     *
     * @param buffer считанные из COM-порта данные
     * @return данные, подлежащие де-кодированию и де-байт-стаффингу
     */
    private static byte[] checkReceivedFrame(byte[] buffer) {
        List<Byte> frame_checked = new ArrayList<>();

        for (int i = 0; i < buffer.length; i++) {
            if (i == 0 && buffer[i] == '#') {
                frame_checked.add(buffer[0]);
            } else if (i == 6)
                break;
            else
                frame_checked.add(buffer[i]);
        }

        byte[] data_checked = new byte[frame_checked.size()];
        for (int i = 0; i < frame_checked.size(); i++)
            data_checked[i] = frame_checked.get(i);

        LabWork1.debug_area.append("Data checked: " + Arrays.toString(data_checked) + "\n");

        return data_checked;
    }

    /**
     * Метод для обновления настроек COM-порта и/или самого порта.
     *
     * @param label_message текстовое поле для обновления информации об осуществленном действии
     */
    private static void updateSerialPort(JLabel label_message) {
        if (!PortAccessManager.tryOpenPort("COM" + selected_port_num)) {
            label_message.setText(label_message.getText() + ". \nCOM" + selected_port_num + " is already opened");
            debug_area.append("COM-port can't be opened or has already been opened\n");

            int previous = Integer.parseInt(com_port.getSystemPortName().replaceAll("\\D", ""));
            com_port_number.setSelectedItem(previous);

            if (flag == 0) return;
        }

        if (flag == 0) {
            input_area.setText("");
            output_area.setText("");
            label_frame.setText("         Frame: ");
            label_total.setText("         Bytes received in total: ");
            debug_area.setText("");
            bytes_received_num = 0;
        }

        if (com_port.isOpen()) {
            com_port.closePort();
            PortAccessManager.closePort(com_port.getSystemPortName());
            debug_area.append(com_port.getSystemPortName() + " has been closed\n");
        }

        com_port = SerialPort.getCommPort("COM" + selected_port_num);
        com_port.setNumStopBits(selected_stopbits);

        if (!com_port.isOpen()) {
            if (!com_port.openPort()) {
                debug_area.append("COM-port can't be opened, because it probably disconnected\n");
                label_message.setText(label_message.getText() + ". \nCOM" + selected_port_num + " can't be opened");
            }
            PortAccessManager.tryOpenPort("COM" + selected_port_num);
            debug_area.append(com_port.getSystemPortName() + " has been opened\n");
        } else
            debug_area.append("COM-port can't be opened or has already been opened\n");
    }

    public static void main(String[] args) {
        setUpPort();

        /*
          Блок кода, отвечающий за создание интерфейса.
         */

        JFrame main_window = new JFrame("Communication program");
        main_window.setSize(800, 600);
        main_window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        JPanel main_panel = new JPanel(new GridBagLayout());
        main_panel.setBackground(new Color(173, 216, 230));

        JPanel[] panels = new JPanel[5];
        for (int i = 0; i < 5; i++) {
            panels[i] = new JPanel();
        }

        for (int i = 0; i < 5; i++) {
            panels[i].setBackground(new Color(173, 216, 230));

            panels[i].setBorder(BorderFactory.createLineBorder(Color.BLACK));
        }

        input_area = new JTextArea();
        output_area = new JTextArea();
        label_frame = new JLabel("         Frame: ");
        debug_area = new JTextArea();

        GridBagConstraints params = new GridBagConstraints();

        /*
          Блок кода, отвечающий за создание ОКНА ВВОДА.
         */

        params.gridx = 0;
        params.gridy = 0;
        params.fill = GridBagConstraints.BOTH;
        params.weightx = 0.6;
        params.weighty = 0.1;

        main_panel.add(panels[0], params);

        input_area.setLineWrap(true);    //перенос строк
        input_area.setWrapStyleWord(true);    //перенос слов
        input_area.setBackground(new Color(173, 216, 230));

        JScrollPane input_scroll = new JScrollPane(input_area);
        main_panel.add(input_scroll, params);

        input_area.setCaretPosition(input_area.getDocument().getLength());

        /*
          Блок кода, отвечающий за создание ОКНА ВЫВОДА.
         */

        params.gridx = 0;
        params.gridy = 1;
        params.weightx = 0.6;
        params.weighty = 0.1;

        main_panel.add(panels[1], params);

        output_area.setEditable(false);
        output_area.setLineWrap(true);
        output_area.setWrapStyleWord(true);
        output_area.setBackground(new Color(173, 216, 230));

        JScrollPane output_scroll = new JScrollPane(output_area);
        output_scroll.setPreferredSize(new Dimension(200, 60));
        main_panel.add(output_scroll, params);

        /*
          Блок кода, отвечающий за создание ОКНА УПРАВЛЕНИЯ.
         */

        params.gridx = 1;
        params.gridy = 0;
        params.weightx = 0.05;
        params.weighty = 0.1;

        main_panel.add(panels[2], params);

        panels[2].setLayout(new GridBagLayout());

        Integer[] port_numbers_array = getListOfAvailablePorts();
        com_port_number = new JComboBox<>(port_numbers_array);

        params.gridx = 1;
        params.gridy = 0;
        params.fill = GridBagConstraints.NONE;
        panels[2].add(com_port_number, params);

        JComboBox<Integer> stopbits = new JComboBox<>(new Integer[]{1, 2});

        params.gridx = 1;
        params.gridy = 1;
        params.fill = GridBagConstraints.NONE;
        panels[2].add(stopbits, params);

        JLabel label_ports = new JLabel("Select COM-port:");
        Font newFont = new Font(label_ports.getFont().getName(), Font.BOLD, 14);
        params.gridx = 0;
        params.gridy = 0;
        label_ports.setFont(newFont);
        panels[2].add(label_ports, params);

        JLabel label_bits = new JLabel("Select stopbits:   ");
        params.gridx = 0;
        params.gridy = 1;
        label_bits.setFont(newFont);
        panels[2].add(label_bits, params);

        JLabel label_message = new JLabel("  COM1 and 1 stopbit by default");
        if (SerialPort.getCommPorts().length == 0)
            label_message.setText("  No ports available");
        params.gridx = 0;
        params.gridy = 2;
        params.gridwidth = 2;
        params.fill = GridBagConstraints.NONE;
        label_message.setForeground(Color.RED);
        label_message.setPreferredSize(new Dimension(300, label_message.getPreferredSize().height));
        panels[2].add(label_message, params);

        /*
          Блок кода, отвечающий за создание ОКНА СОСТОЯНИЯ.
         */

        params.gridx = 1;
        params.gridy = 1;
        params.fill = GridBagConstraints.BOTH;

        main_panel.add(panels[3], params);

        panels[3].setLayout(new GridBagLayout());

        JLabel label_speed = new JLabel("Speed: " + com_port.getBaudRate() + "                                 ");
        params.gridx = 0;
        params.gridy = 0;
        params.fill = GridBagConstraints.NONE;
        label_speed.setFont(newFont);
        panels[3].add(label_speed, params);

        params.gridx = 0;
        params.gridy = 1;
        params.fill = GridBagConstraints.HORIZONTAL;
        label_frame.setFont(newFont);
        label_frame.setHorizontalAlignment(SwingConstants.LEFT);
        label_frame.setVerticalTextPosition(SwingConstants.TOP);
        panels[3].add(label_frame, params);

        label_total = new JLabel("         Bytes received in total: ");
        params.gridx = 0;
        params.gridy = 2;
        params.fill = GridBagConstraints.HORIZONTAL;
        label_total.setFont(newFont);
        label_total.setHorizontalAlignment(SwingConstants.LEFT);
        panels[3].add(label_total, params);

        /*
          Блок кода, отвечающий за создание ОКНА ОТЛАДКИ.
         */

        params.gridx = 0;
        params.gridy = 2;
        params.gridwidth = 3;
        params.gridheight = 1;
        params.fill = GridBagConstraints.BOTH;
        main_panel.add(panels[4], params);

        panels[4].setLayout(new GridBagLayout());

        debug_area.setEditable(false);
        debug_area.setLineWrap(true);
        debug_area.setWrapStyleWord(true);
        debug_area.setBackground(new Color(173, 216, 230));

        JScrollPane debug_scroll = new JScrollPane(debug_area);
        debug_scroll.setPreferredSize(new Dimension(300, 50));
        debug_scroll.setBorder(null);
        panels[4].add(debug_scroll, params);

        main_window.add(main_panel);

        openPortByDefault(label_message);

        /*
          Блок кода, отвечающий за реализацию слушателей.
         */

        input_area.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                int keyCode = e.getKeyCode();
                if (keyCode == KeyEvent.VK_LEFT || keyCode == KeyEvent.VK_RIGHT || keyCode == KeyEvent.VK_UP || keyCode == KeyEvent.VK_DOWN) {
                    e.consume();
                }
            }
        });

        input_area.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                int keyCode = e.getKeyCode();
                if (keyCode == KeyEvent.VK_BACK_SPACE) {
                    e.consume();
                }
            }
        });

        input_area.getDocument().addDocumentListener(new DocumentListener() {
            @Override
            public void insertUpdate(DocumentEvent e) {
                String text = input_area.getText();

                String data_sent = text.substring(text.length() - DATA_LENGTH);
                byte[] bytes_sent;
                try {
                    bytes_sent = data_sent.getBytes("CP1251");
                } catch (UnsupportedEncodingException ex) {
                    throw new RuntimeException(ex);
                }
                debug_area.append("\nData to send: " + data_sent + "\n");

                Frame frame = new Frame();
                frame.formFrame(DATA_LENGTH, selected_port_num);
                frame.setData(bytes_sent);
                byte[] frame_encoded = frame.encodeFrame();

                printFrame(frame_encoded);

                int attempts = 0;
                Random rand = new Random();
                boolean flag_repeat = true;
                byte[] jam = {(byte) 0xAA, (byte) 0xBB, (byte) 0xCC, (byte) 0xDD, (byte) 0xEE, (byte) 0xFF};

                while (flag_repeat) {    //30%-вероятность занятости канала
                    if (rand.nextInt(100) < 30) {    // || !com_port.getCTS()
                        debug_area.append("The channel is occupied\n");

                        continue;
                    }

                    com_port.writeBytes(frame_encoded, frame_encoded.length);
                    debug_area.append("Data encoded: " + Arrays.toString(frame_encoded) + "\n");

//                    byte[] response = new byte[1024];
//                    int bytes_check = com_port.readBytes(response, response.length);

                    if (rand.nextInt(100) < 70) {    // || bytes_check == 0, 70%-вероятность возникновения коллизии
                        debug_area.append("Collision detected\n");
                        label_frame.setText(label_frame.getText() + (attempts == 0 ? " " : "") + "c");

                        debug_area.append("Sending jam signal...\n");
                        com_port.writeBytes(jam, jam.length);

                        attempts++;

                        if (attempts > 10) {
                            debug_area.append("Too many collisions - aborting\n");
                            label_frame.setText("<html>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + label_frame.getText().substring(0, 26) + "<font color='red'> ccccccccccc</font></html>");

                            return;
                        }

                        int k = Math.min(attempts, 10);
                        int r = rand.nextInt((int) Math.pow(2, k));    //количество слот-таймов
                        int slot_times = r;    //8/115200 + (3*8)/115200

                        try {
                            Thread.sleep(slot_times);
                        } catch (InterruptedException ex) {
                            throw new RuntimeException(ex);
                        }

                        debug_area.append("Backoff for " + r + " slot-times started\n");
                    } else flag_repeat = false;
                }
            }

            @Override
            public void removeUpdate(DocumentEvent e) {
            }

            @Override
            public void changedUpdate(DocumentEvent e) {
            }
        });

        input_area.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                input_area.setCaretPosition(input_area.getDocument().getLength());
            }
        });

        com_port_number.addActionListener(e -> {
            Integer value = (Integer) com_port_number.getSelectedItem();

            if (value != null) {
                selected_port_num = value;

                label_message.setText("  COM" + selected_port_num + " was selected");
                updateSerialPort(label_message);
            }
        });

        stopbits.addActionListener(e -> {
            Integer value = (Integer) stopbits.getSelectedItem();

            if (value != null) {
                flag = 1;
                selected_stopbits = value;

                updateSerialPort(label_message);
                label_message.setText("  " + selected_stopbits + " stopbit(s)" + " selected");
                flag = 0;
            }
        });

        /*
          Блок кода, отвечающий за чтение ввода данных в режиме реального времени.
         */

        Timer timer_read = new Timer(0, e -> {
            byte[] buffer = new byte[6];
            int bytes_read = com_port.readBytes(buffer, buffer.length);

            if (bytes_read > 0) {
                processReceivedData(buffer);
            }
        });
        timer_read.start();

        main_window.setVisible(true);

        main_window.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                com_port.closePort();
                PortAccessManager.closePort(com_port.getSystemPortName());
            }
        });
    }

    /**
     * Метод для считывания данных из COM-порта и проверки на наличие ошибок или возникновение коллизии.
     * @param buffer буфер данных, считанных из COM-порта
     */
    private static void processReceivedData(byte[] buffer) {
        byte[] jam = {(byte) 0xAA, (byte) 0xBB, (byte) 0xCC, (byte) 0xDD, (byte) 0xEE, (byte) 0xFF};

        debug_area.append("\n");

        for (int i = 0; i <= buffer.length - jam.length; i++) {
            if (Arrays.equals(Arrays.copyOfRange(buffer, i, i + jam.length), jam))    //если пришел jam-сигнал
            {
                    debug_area.append("Jam signal detected\n\n");

                    output_area.setText(output_area.getText().substring(0, output_area.getText().length() - 1));    //вероятно данные пришли раньше и уже вывелись
                    debug_area.append("Previous frame was rejected\n");

                    return;    //прекратить прием данных
            }
        }

        byte[] data_checked;
        data_checked = checkReceivedFrame(buffer);

        byte[] data_decoded = new Frame().decodeFrame(data_checked, data_checked.length);
        debug_area.append("Data decoded: " + Arrays.toString(data_decoded) + "\n");

        if (data_decoded == null)
            return;
        if (Frame.error == 1)
            label_total.setText("<html>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Bytes received in total: " + bytes_received_num + "<font color='red'> *</font></html>");

        String data_received;
        try {
            data_received = new String(data_decoded, "CP1251");
        } catch (UnsupportedEncodingException ex) {
            throw new RuntimeException(ex);
        }

        output_area.append(data_received);

        bytes_received_num += data_received.length();
        if (Frame.error == 0)
            label_total.setText("         Bytes received in total: " + bytes_received_num);
    }
}