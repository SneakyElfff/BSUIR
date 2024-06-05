//import com.fazecast.jSerialComm.SerialPort;
//import com.fazecast.jSerialComm.SerialPortEvent;
//import com.fazecast.jSerialComm.SerialPortMessageListener;
//
//import javax.swing.*;
//import java.util.ArrayList;
//import java.util.Arrays;
//import java.util.List;
//
//public class PortDataListener implements SerialPortMessageListener {
//    private final JTextArea output_area;
//
//    public PortDataListener(JTextArea output_area) {
//        this.output_area = output_area;
//    }
//
//    @Override
//    public int getListeningEvents() {
//        return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
//    }
//
//    @Override
//    public void serialEvent(SerialPortEvent event) {
//        if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE)
//            return;
//
//        byte[] data_new = new byte[event.getSerialPort().bytesAvailable()];
//        event.getSerialPort().readBytes(data_new, data_new.length);
//        LabWork1.debug_area.append("Data received: " + Arrays.toString(data_new) + "\n");
//
//        List<Byte> frame_checked = new ArrayList<>();
//
//        for (int i=0; i<data_new.length; i++) {
//            if (i == 0 && data_new[i] == '#') {
//                frame_checked.add(data_new[0]);
//                continue;
//            }
//            else if (data_new[i] == 0 && data_new[i-1] != 0) {
//                frame_checked.add(data_new[i]);
//                break;
//            }
//            else
//                frame_checked.add(data_new[i]);
//        }
//
//        byte[] data_checked = new byte[frame_checked.size()];
//        for (int i = 0; i < frame_checked.size(); i++)
//            data_checked[i] = frame_checked.get(i);
//
//        LabWork1.debug_area.append("Data checked: " + Arrays.toString(data_checked) + "\n");
//
//        byte[] data_destuffed = new Frame().decodeFrame(data_checked, data_checked.length);
//        LabWork1.debug_area.append("Data destuffed: " + Arrays.toString(data_destuffed) + "\n");
//
//        String data_received = new String(data_destuffed);
//
//        SwingUtilities.invokeLater(() -> output_area.append(data_received));
//
//        LabWork1.bytes_received_num++;
//        LabWork1.state_area.setText("           Bytes received in total: " + String.valueOf(LabWork1.bytes_received_num));
//    }
//
//    @Override
//    public byte[] getMessageDelimiter() {
//        return new byte[0];
//    }
//
//    @Override
//    public boolean delimiterIndicatesEndOfMessage() {
//        return false;
//    }
//}