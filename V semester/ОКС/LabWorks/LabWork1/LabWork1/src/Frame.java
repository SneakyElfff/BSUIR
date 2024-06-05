import java.util.Arrays;
import java.util.Random;

public class Frame {
    private String flag;
    private String address_destination;
    private String address_source;
    private byte[] data;
    private int FCS = 0;
    static int error = 0;

    /**
     * Метод для создания структуры кадра.
     * @param num размер поля данных
     * @param port номер COM-порта
     */
    public void formFrame(int num, int port) {
        this.flag = "#" + "a";
        this.address_destination = "0";
        this.address_source = String.valueOf(port);
        this.data = new byte[num];
        this.FCS = 0;
    }

    /**
     * Метод для заполнения поля данных.
     * @param data передаваемые данные
     */
    public void setData(byte[] data) {
        if(data.length == this.data.length) {
            System.arraycopy(data, 0, this.data, 0, data.length);
            this.FCS = calculateFCS(data);
        }
        else
            LabWork1.debug_area.append("Data length must be " + this.data.length + " bytes\n");
    }

    /**
     * Метод для вычисления контрольной суммы с применением циклического кода CRC-8.
     * @param data передаваемые данные
     * @return контрольная сумма
     */
    private byte calculateFCS(byte[] data) {
        int CRC = 0;    //CRC shift register
        int generator_polynomial = 0b110;    //x^2+x, CRC-3

        for (byte datum : data) {
            CRC = datum;
            for (int i=0; i<8; i++) {
                if ((CRC & 1) != 0)    //если LSB != 0
                    CRC = (CRC >> 1) ^ generator_polynomial;
                else CRC >>= 1;
            }
        }

        return (byte) (CRC);
    }

    /**
     * Метод для кодирования кадра и осуществления байт-стаффинга (если необходимо).
     * @return сформированный кадр, готовый к отправке
     */
    public byte[] encodeFrame() {
        int frame_size = 2 + 1 + 1 + data.length + 1;
        byte[] frame = new byte[frame_size];

        frame[0] = (byte) flag.charAt(0);
        frame[1] = (byte) flag.charAt(1);
        frame[2] = (byte) address_destination.charAt(0);
        frame[3] = (byte) address_source.charAt(0);
        frame[frame_size-1] = (byte) FCS;

        int data_index = 4;

        for (byte datum : data)
            frame[data_index++] = datum;

        return frame;
    }

    /**
     * Метод для валидации кадра и проверки наличия его основных полей.
     * @param frame_received полученный кадр
     * @param size размер кадра
     * @return true - структура кадра верная;
     * false - структура кадра нарушена
     */
    public boolean validateFrame(byte[] frame_received, int size) {
        if (size != 6)
            return false;

        return frame_received[0] == (byte) '#' || frame_received[1] == (byte) 'a';
    }

    /**
     * Метод для декодирования кадра и проверки контрольной суммы.
     * @param frame_received полученный кадр
     * @param size размер полученного кадра
     * @return полученные данные после проверки
     */
    public byte[] decodeFrame(byte[] frame_received, int size) {
        if (!validateFrame(frame_received, size)) {
            LabWork1.debug_area.append("Frame " + Arrays.toString(frame_received) + " is invalid\n");
            return null;
        }

        byte[] data_decoded = new byte[LabWork1.DATA_LENGTH];
        for (int i=4, j=0; i<frame_received.length-1; i++, j++)
            data_decoded[j] = frame_received[i];

        data_decoded = distortRandomBit(data_decoded);

        byte CRC_calculated = calculateFCS(data_decoded);
        LabWork1.debug_area.append("FCS calculated: " + String.format("%8s", Integer.toBinaryString(CRC_calculated & 0xFF)).replace(' ', '0') + "\n");
        Frame.error = 0;
        if (CRC_calculated != frame_received[size - 1]) {
            LabWork1.debug_area.append("Data were corrupted: wrong FCS\n");
            error = 1;

            byte[] data_corrected = Arrays.copyOf(data_decoded, data_decoded.length);
            for (int i=0; i<8; i++) {
                byte original = data_corrected[0];
                data_corrected[0] ^= (byte) (1 << i);    //инвертируем бит

                byte CRC_corrected = calculateFCS(data_corrected);

                if (CRC_corrected == frame_received[size - 1]) {
                    LabWork1.debug_area.append("Single-bit error corrected at bit " + i + "\n");
                    data_decoded = data_corrected;
                    break;
                } else {
                    data_corrected[0] = original;
                }
            }
        }

        return data_decoded;
    }

    /**
     * Метод для искажения случайного бита в данных.
     * @param data_decoded полученные данные
     * @return данные с возможным искажением
     */
    private static byte[] distortRandomBit(byte[] data_decoded) {
        Random random = new Random();

        byte[] data_distorted = new byte[data_decoded.length];
        System.arraycopy(data_decoded, 0, data_distorted, 0, data_decoded.length);

        for (int j=0; j<data_decoded.length; j++) {
            if (random.nextInt(10) < 3) {
                int bit = random.nextInt(8);

                byte mask = (byte) (1 << bit);    //1 только на месте bit

                byte result = (byte) (data_decoded[j] ^ mask);

                data_distorted[j] = result;
                LabWork1.debug_area.append("Data were distorted\n");

                return data_distorted;
            }
            else return data_decoded;
        }

        return data_decoded;
    }
}