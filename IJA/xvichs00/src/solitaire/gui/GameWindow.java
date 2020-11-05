package solitaire.gui;

import solitaire.model.board.*;
import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import solitaire.model.cards.*;
import java.io.Serializable;

/**
 * Class represents GameWindow.
 *
 * @author Stepan Vich
 * @author Marek Rohel
 */
public class GameWindow extends JPanel implements Serializable {

    public WorkingDeckG activeWorkingDeck;
    public int workingDeckCardIndex;
    public TargetDeckG activeTargetDeck;
    public MainDeckRestG activeMainDeckRest;

    public MainDeckG mainDeckG;
    public MainDeckRestG mainDeckRestG;
    public TargetDeckG targetDeck1G;
    public TargetDeckG targetDeck2G;
    public TargetDeckG targetDeck3G;
    public TargetDeckG targetDeck4G;
    public LinkedList<WorkingDeckG> workDecksG;

    public final int glue = 5;

    /**
     * Sets game parent.
     *
     * @param parent parent
     */
    public void setParent(GameGrid parent) {
        this.parent = parent;
    }
    public GameWindow refference;
    protected GameGrid parent;

    /**
     * Cheks if the game is won.
     */
    public void checkWin() {
        if (targetDeck1G.deck.size() == 13 && targetDeck2G.deck.size() == 13 && targetDeck3G.deck.size() == 13 && targetDeck4G.deck.size() == 13) {
            // Game won...
            gameWonLabel.setText("Game Won!!!");

        } else {
            gameWonLabel.setText("");
        }
    }

    /**
     * Fills the decks.
     */
    public void fillDecks() {
        // Adding main deck.
        FactoryKlondike factory = new FactoryKlondike();

        CardDeck mainDeck = factory.createStandardDeck();
        mainDeck.shuffle();
        mainDeckG.setDeck(mainDeck);

        CardDeck restDeck = new CardDeck();
        mainDeckRestG.setDeck(restDeck);

        TargetDeck targetDeck1 = factory.createTargetDeck(Card.Color.CLUBS);
        TargetDeck targetDeck2 = factory.createTargetDeck(Card.Color.DIAMONDS);
        TargetDeck targetDeck3 = factory.createTargetDeck(Card.Color.HEARTS);
        TargetDeck targetDeck4 = factory.createTargetDeck(Card.Color.SPADES);
        targetDeck1G.setDeck(targetDeck1);
        targetDeck2G.setDeck(targetDeck2);
        targetDeck3G.setDeck(targetDeck3);
        targetDeck4G.setDeck(targetDeck4);

        LinkedList<WorkingDeck> workDecks = new LinkedList<>();

        // Creating 7 working decks.
        for (int i = 0; i < 7; i++) {
            // Filling work decks.
            workDecks.addLast(factory.createWorkingDeck());
            // Card put.
            workDecks.getLast().put(mainDeck.pop(i + 1));
            // Turning up card on top.
            workDecks.get(i).get().turnFaceUp();
            workDecksG.get(i).setDeck(workDecks.get(i));
        }
    }

    public LinkedList<GameSnap> moves;

    /**
     * Undo last move.
     */
    public void undo() {
        if (moves.size() == 0) {    // ----------------------------------- moves.isEmpty()
            return;
        }

        // Deleting active elements
        activeWorkingDeck = null;
        activeTargetDeck = null;
        activeMainDeckRest = null;

        // Undo.
        GameSnap snap = moves.removeLast();
        mainDeckG.setDeck(snap.mainDeck);
        mainDeckRestG.setDeck(snap.mainDeckRest);
        targetDeck1G.setDeck(snap.targetDeck1);
        targetDeck2G.setDeck(snap.targetDeck2);
        targetDeck3G.setDeck(snap.targetDeck3);
        targetDeck4G.setDeck(snap.targetDeck4);
        targetDeck4G.setDeck(snap.targetDeck4);

        // Replace 7 working decks.
        for (int i = 0; i < 7; i++) {
            workDecksG.get(i).setDeck(snap.workDecks.get(i));
        }

    }

    public WorkingDeckG hintWorkingDeck;
    public WorkingDeckG hintWorkingDeckTo;
    public TargetDeckG hintTargetDeck;
    public MainDeckG hintMainDeck;
    public MainDeckRestG hintMainDeckRest;
    public int hintIndexOfCard;

    /**
     * Hints move.
     */
    public void hint() {
        // Search all cards on top of the working packs and checks if they can be added to target packs.
        for (int m = 0; m < 7; m++) {

            Card topCard = workDecksG.get(m).deck.get();
            if (topCard != null) {
                if (targetDeck1G.deck.put(topCard)) {
                    // Card can be added -> put card back and print hint.
                    targetDeck1G.deck.pop();
                    hintTargetDeck = targetDeck1G;
                    hintWorkingDeck = workDecksG.get(m);
                    hintIndexOfCard = workDecksG.get(m).deck.size() - 1;
                    return;
                }
                if (targetDeck2G.deck.put(topCard)) {
                    // Card can be added -> put card back and print hint.
                    targetDeck2G.deck.pop();
                    hintTargetDeck = targetDeck2G;
                    hintWorkingDeck = workDecksG.get(m);
                    hintIndexOfCard = workDecksG.get(m).deck.size() - 1;
                    return;
                }
                if (targetDeck3G.deck.put(topCard)) {
                    // Card can be added -> put card back and print hint.
                    targetDeck3G.deck.pop();
                    hintTargetDeck = targetDeck3G;
                    hintWorkingDeck = workDecksG.get(m);
                    hintIndexOfCard = workDecksG.get(m).deck.size() - 1;
                    return;
                }
                if (targetDeck4G.deck.put(topCard)) {
                    // Card can be added -> put card back and print hint.
                    targetDeck4G.deck.pop();
                    hintTargetDeck = targetDeck4G;
                    hintWorkingDeck = workDecksG.get(m);
                    hintIndexOfCard = workDecksG.get(m).deck.size() - 1;
                    return;
                }
            }

        }

        //  Search all cards on top of the target packs and checks if they can be added to goal (finish) pack.
        Card topCard = mainDeckRestG.deck.get();
        if (topCard != null) {
            if (targetDeck1G.deck.put(topCard)) {
                // Card can be added -> put card back and print hint.
                targetDeck1G.deck.pop();
                hintTargetDeck = targetDeck1G;
                hintMainDeckRest = mainDeckRestG;
                return;
            }
            if (targetDeck2G.deck.put(topCard)) {
                // Card can be added -> put card back and print hint.
                targetDeck2G.deck.pop();
                hintTargetDeck = targetDeck2G;
                hintMainDeckRest = mainDeckRestG;
                return;
            }
            if (targetDeck3G.deck.put(topCard)) {
                // Card can be added -> put card back and print hint.
                targetDeck3G.deck.pop();
                hintTargetDeck = targetDeck3G;
                hintMainDeckRest = mainDeckRestG;
                return;
            }
            if (targetDeck4G.deck.put(topCard)) {
                // Card can be added -> put card back and print hint.
                targetDeck4G.deck.pop();
                hintTargetDeck = targetDeck4G;
                hintMainDeckRest = mainDeckRestG;
                return;
            }
        }

        // Checks if card from deck can be moved to working deck.
        if (mainDeckRestG.deck.size() > 0) {
            for (int i = 0; i < 7; i++) {
                if (workDecksG.get(i).deck.put(mainDeckRestG.deck.get())) {
                    workDecksG.get(i).deck.pop();
                    hintMainDeckRest = mainDeckRestG;
                    hintWorkingDeck = workDecksG.get(i);
                    hintIndexOfCard = -1;
                    return;
                }
            }
        }

        // Maps to all top cards from working pack, card which is turned up and its value is 1 higher (and is in working deck or deck). ->
        // -> Moves card or card stack to working decks.
        // We've to check not to move between sequences.
        for (int m = 0; m < 7; m++) {
            int cardsInWorkDeck = workDecksG.get(m).deck.size();
            for (int n = 0; n < cardsInWorkDeck; n++) {
                Card currentCard = workDecksG.get(m).deck.get(n);
                if (!currentCard.isTurnedFaceUp()) {
                    continue;
                }
                // Check the top of the rest of working packs.
                for (int i = 0; i < 7; i++) {
                    if (i == m) {
                        continue;
                    }
                    // If card is first and it's king, delete this hint.
                    if (n == 0 && currentCard.value() == 13) {
                        continue;
                    }
                    if (workDecksG.get(i).deck.put(currentCard)) {
                        workDecksG.get(i).deck.pop();
                        hintWorkingDeck = workDecksG.get(m);
                        hintWorkingDeckTo = workDecksG.get(i);
                        hintIndexOfCard = n;
                        return;
                    }
                }
                // We do not have to check other cards, break cycle on this working deck.
                break;
            }
        }

        // Otherwise we can click on main deck - if it's not empty.
        if (mainDeckG.deck.size() > 0 || mainDeckRestG.deck.size() > 0) {
            hintMainDeck = mainDeckG;
        }
    }

    /**
     * Repaints all cards.
     */
    public void repaintAll() {
        targetDeck1G.repaint();
        targetDeck2G.repaint();
        targetDeck3G.repaint();
        targetDeck4G.repaint();
        mainDeckG.repaint();
        mainDeckRestG.repaint();
        for (int i = 0; i < workDecksG.size(); i++) {
            workDecksG.get(i).repaint();
        }
    }

    /**
     * Copies decks.
     *
     * @return Game window.
     */
    public GameWindow copyDecks() {
        GameWindow gameWindow = new GameWindow();

        try {
            gameWindow.mainDeckG.deck = (CardDeck) ObjectCloner.deepCopy(mainDeckG.deck);
            gameWindow.mainDeckRestG = (MainDeckRestG) ObjectCloner.deepCopy(mainDeckRestG);
            gameWindow.targetDeck1G = (TargetDeckG) ObjectCloner.deepCopy(targetDeck1G);
            gameWindow.targetDeck2G = (TargetDeckG) ObjectCloner.deepCopy(targetDeck2G);
            gameWindow.targetDeck3G = (TargetDeckG) ObjectCloner.deepCopy(targetDeck3G);
            gameWindow.targetDeck4G = (TargetDeckG) ObjectCloner.deepCopy(targetDeck4G);
            for (int i = 0; i < workDecksG.size(); i++) {
                gameWindow.workDecksG.set(i, (WorkingDeckG) ObjectCloner.deepCopy(workDecksG.get(i)));
            }
        } catch (Exception exception) {

        }

        return gameWindow;

    }

    /**
     * Replaces decks.
     *
     * @param newSnap New snap.
     */
    public void replaceDecks(GameSnap newSnap) {

        mainDeckG.setDeck(newSnap.mainDeck);

        mainDeckRestG.setDeck(newSnap.mainDeckRest);

        targetDeck1G.setDeck(newSnap.targetDeck1);
        targetDeck2G.setDeck(newSnap.targetDeck2);
        targetDeck3G.setDeck(newSnap.targetDeck3);
        targetDeck4G.setDeck(newSnap.targetDeck4);

        // Replace 7 working decks.
        for (int i = 0; i < 7; i++) {
            workDecksG.get(i).setDeck(newSnap.workDecks.get(i));
        }
    }
    public JLabel gameWonLabel;

    /**
     * Constructor.
     */
    public GameWindow() {
        // Helper.
        refference = this;

        // Setting background color.
        setBackground(Color.GREEN);

        // Sets main location.
        setLocation(200, 200);

        // Sets main layout - we'll store three main elements.
        setLayout(new BorderLayout());

        // Initialization of list.
        moves = new LinkedList();

        // Adding main deck.
        FactoryKlondike factory = new FactoryKlondike();
        mainDeckG = new MainDeckG();
        CardDeck mainDeck = factory.createStandardDeck();
        mainDeck.shuffle();
        mainDeckG.setDeck(mainDeck);
        mainDeckG.setParent(this);

        // Adding deck.
        mainDeckRestG = new MainDeckRestG();
        CardDeck restDeck = new CardDeck();
        mainDeckRestG.setDeck(restDeck);
        mainDeckRestG.setParent(this);

        // Adding target decks.
        TargetDeck targetDeck1 = factory.createTargetDeck(Card.Color.CLUBS);
        TargetDeck targetDeck2 = factory.createTargetDeck(Card.Color.DIAMONDS);
        TargetDeck targetDeck3 = factory.createTargetDeck(Card.Color.HEARTS);
        TargetDeck targetDeck4 = factory.createTargetDeck(Card.Color.SPADES);
        targetDeck1G = new TargetDeckG();
        targetDeck1G.setDeck(targetDeck1);
        targetDeck1G.setParent(this);
        targetDeck2G = new TargetDeckG();
        targetDeck2G.setDeck(targetDeck2);
        targetDeck2G.setParent(this);
        targetDeck3G = new TargetDeckG();
        targetDeck3G.setDeck(targetDeck3);
        targetDeck3G.setParent(this);
        targetDeck4G = new TargetDeckG();
        targetDeck4G.setDeck(targetDeck4);
        targetDeck4G.setParent(this);

        // Adding first row.	
        JPanel topPanel = new JPanel();
        topPanel.setBackground(Color.GREEN);
        topPanel.setLayout(new BoxLayout(topPanel, BoxLayout.X_AXIS));
        topPanel.add(Box.createRigidArea(new Dimension(glue, 0)));
        topPanel.add(mainDeckG);
        topPanel.add(Box.createRigidArea(new Dimension(glue, 0)));
        topPanel.add(mainDeckRestG);
        topPanel.add(Box.createRigidArea(new Dimension(glue, 0)));
        // Adding empty deck for printing gap between main decks.
        topPanel.add(new CardDeckG());
        topPanel.add(Box.createRigidArea(new Dimension(glue, 0)));

        topPanel.add(targetDeck1G);
        topPanel.add(Box.createRigidArea(new Dimension(glue, 0)));

        topPanel.add(targetDeck2G);
        topPanel.add(Box.createRigidArea(new Dimension(glue, 0)));

        topPanel.add(targetDeck3G);
        topPanel.add(Box.createRigidArea(new Dimension(glue, 0)));

        topPanel.add(targetDeck4G);
        topPanel.add(Box.createRigidArea(new Dimension(glue, 0)));

        // Adding the lowest row	
        JPanel centerPanel = new JPanel();
        centerPanel.setBackground(Color.GREEN);
        centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.X_AXIS));

        // Creating working deck.
        LinkedList<WorkingDeck> workDecks = new LinkedList<>();
        workDecksG = new LinkedList<>();
        centerPanel.add(Box.createRigidArea(new Dimension(glue, 0)));

        // Creating 7 working decks.
        for (int i = 0; i < 7; i++) {
            // Filling working decks.
            workDecks.addLast(factory.createWorkingDeck());
            // Card move.
            workDecks.get(i).put(mainDeck.pop(i + 1));
            // Turn up card on top.
            workDecks.get(i).get().turnFaceUp();

            WorkingDeckG workDeckG = new WorkingDeckG();
            workDeckG.setDeck(workDecks.get(i));
            workDeckG.setParent(this);
            workDecksG.addLast(workDeckG);
            centerPanel.add(workDeckG);
            centerPanel.setBorder(BorderFactory.createMatteBorder(glue, 0, 0, 0, Color.GREEN));
        }

        // Creating control panels.
        JPanel bottomPanel = new JPanel();
        JPanel bottomNorthPanel = new JPanel();
        JPanel bottomLeftPanel = new JPanel();
        JPanel bottomCenterPanel = new JPanel();
        JPanel bottomRightPanel = new JPanel();

        // Filling JLabel text.
        gameWonLabel = new JLabel("");
        gameWonLabel.setBackground(Color.GREEN);
        bottomNorthPanel.add(gameWonLabel);
        bottomNorthPanel.setBackground(Color.GREEN);

        bottomPanel.setLayout(new BorderLayout());
        bottomLeftPanel.setLayout(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        bottomCenterPanel.setLayout(new GridBagLayout());
        bottomRightPanel.setLayout(new GridBagLayout());

        bottomPanel.setBackground(Color.GREEN);
        bottomLeftPanel.setBackground(Color.GREEN);
        bottomCenterPanel.setBackground(Color.GREEN);
        bottomRightPanel.setBackground(Color.GREEN);
        // Filling row with control panels.
        JButton newGameButton = new JButton("New Game");
        newGameButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                fillDecks();
                checkWin();
            }
        });
        bottomLeftPanel.add(newGameButton, gbc);

        JButton saveGameButton = new JButton("Save Game");
        saveGameButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                parent.saveGame(new GameSnap(refference));
            }
        });
        bottomLeftPanel.add(saveGameButton, gbc);

        JButton loadGameButton = new JButton("Load Game");
        loadGameButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                parent.loadGame(refference);
                checkWin();
            }
        });
        bottomLeftPanel.add(loadGameButton, gbc);

        JButton undoButton = new JButton("Undo");
        undoButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                undo();
                checkWin();
            }
        });
        bottomCenterPanel.add(undoButton, gbc);

        JButton hintButton = new JButton("Hint");
        hintButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                hint();
                repaintAll();
            }
        });
        bottomCenterPanel.add(hintButton, gbc);

        JButton closeTableButton = new JButton("Close Table");
        closeTableButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                parent.closeTable(refference);
            }
        });
        bottomRightPanel.add(closeTableButton, gbc);

        JButton addTableButton = new JButton("Add Table");
        addTableButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                parent.addTable();
            }
        });
        bottomRightPanel.add(addTableButton, gbc);

        // Setting top edge
        topPanel.setBorder(BorderFactory.createMatteBorder(glue, 0, 0, 0, Color.GREEN));
        // Adding top row
        add(topPanel, BorderLayout.NORTH);
        // Adding mid row
        add(centerPanel, BorderLayout.CENTER);
        // Adding bottom row
        add(bottomPanel, BorderLayout.SOUTH);
        bottomPanel.add(bottomNorthPanel, BorderLayout.NORTH);
        bottomPanel.add(bottomLeftPanel, BorderLayout.WEST);
        bottomPanel.add(bottomCenterPanel, BorderLayout.CENTER);
        bottomPanel.add(bottomRightPanel, BorderLayout.EAST);
    }
}
