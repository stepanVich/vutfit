package solitaire.gui;

import java.util.*;
import java.io.Serializable;
import solitaire.model.board.ObjectCloner;
import solitaire.model.cards.*;

/**
 * Class represents GameSnap.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
class GameSnap implements Serializable {

    public CardDeck mainDeck;
    public CardDeck mainDeckRest;
    public TargetDeck targetDeck1;
    public TargetDeck targetDeck2;
    public TargetDeck targetDeck3;
    public TargetDeck targetDeck4;
    public LinkedList<WorkingDeck> workDecks;

    /**
     * Snaps actual game state for save.
     *
     * @param window Game window.
     */
    public GameSnap(GameWindow window) {
        workDecks = new LinkedList();
        try {
            this.mainDeck = (CardDeck) ObjectCloner.deepCopy(window.mainDeckG.deck);
            this.mainDeckRest = (CardDeck) ObjectCloner.deepCopy(window.mainDeckRestG.deck);
            this.targetDeck1 = (TargetDeck) ObjectCloner.deepCopy(window.targetDeck1G.deck);
            this.targetDeck2 = (TargetDeck) ObjectCloner.deepCopy(window.targetDeck2G.deck);
            this.targetDeck3 = (TargetDeck) ObjectCloner.deepCopy(window.targetDeck3G.deck);
            this.targetDeck4 = (TargetDeck) ObjectCloner.deepCopy(window.targetDeck4G.deck);
            int loops = window.workDecksG.size();
            for (int i = 0; i < loops; i++) {
                this.workDecks.addLast((WorkingDeck) ObjectCloner.deepCopy(window.workDecksG.get(i).deck));
            }
        } catch (Exception exception) {

        }
    }

}
