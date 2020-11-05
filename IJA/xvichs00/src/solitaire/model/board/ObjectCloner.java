package solitaire.model.board;

import java.io.*;

/**
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class ObjectCloner {

    /**
     * Constructor
     */
    protected ObjectCloner() {

    }

    /**
     * Copies given object
     *
     * @param object Object to deep copy.
     * @return New object.
     * @throws Exception ClassNotFound or another exceptionn.
     */
    static public Object deepCopy(Object object) throws Exception {
        ObjectOutputStream oos = null;
        ObjectInputStream ois = null;
        try {
            ByteArrayOutputStream bos
                    = new ByteArrayOutputStream();
            oos = new ObjectOutputStream(bos);
            // serialize and pass the object
            oos.writeObject(object);
            oos.flush();
            ByteArrayInputStream bin
                    = new ByteArrayInputStream(bos.toByteArray());
            ois = new ObjectInputStream(bin);
            // return new object
            return ois.readObject();
        } catch (IOException | ClassNotFoundException e) {
            System.out.println("Exception in ObjectCloner = " + e);
            throw (e);
        } finally {
            if (oos != null) {
                oos.close();
            }
            if (ois != null) {
                ois.close();
            }
        }
    }

}
