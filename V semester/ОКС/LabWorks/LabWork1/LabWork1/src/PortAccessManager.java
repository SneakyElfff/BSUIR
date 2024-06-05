import java.io.File;
import java.io.IOException;

public class PortAccessManager {
    private static File lockFile;
    public static boolean tryOpenPort(String name) {
        lockFile = new File(name + ".lock");

        if (!lockFile.exists()) {
            try {
                lockFile.createNewFile();
                return true;
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
        }
        return false;    //port is occupied
    }

    public static void closePort(String name) {
        lockFile = new File(name + ".lock");

        lockFile.delete();
    }
}