import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.util.ArrayList;
import java.util.List;

class Feedback {
    private String name;
    private int rating;
    private String comments;

    public Feedback(String name, int rating, String comments) {
        this.name = name;
        this.rating = rating;
        this.comments = comments;
    }

    public String getName() {
        return name;
    }

    public int getRating() {
        return rating;
    }

    public String getComments() {
        return comments;
    }

    @Override
    public String toString() {
        return "Name: " + name + "\nRating: " + rating + "\nComments: " + comments + "\n";
    }
}

class FeedbackManager {
    private List<Feedback> feedbackList;
    private static final String FILE_PATH = "feedbacks.txt";

    public FeedbackManager() {
        feedbackList = new ArrayList<>();
        loadFeedbacks();
    }

    public void addFeedback(Feedback feedback) {
        feedbackList.add(feedback);
        saveFeedbackToFile(feedback);
    }

    private void saveFeedbackToFile(Feedback feedback) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(FILE_PATH, true))) {
            writer.write(feedback.toString());
            writer.write("-------------\n");
        } catch (IOException e) {
            System.out.println("An error occurred while saving the feedback.");
        }
    }

    private void loadFeedbacks() {
        File file = new File(FILE_PATH);
        if (!file.exists()) return;

        try (BufferedReader reader = new BufferedReader(new FileReader(FILE_PATH))) {
            String line;
            String name = null, comments = null;
            int rating = 0;
            while ((line = reader.readLine()) != null) {
                if (line.startsWith("Name:")) {
                    if (name != null) {
                        feedbackList.add(new Feedback(name, rating, comments));
                    }
                    name = line.split(":")[1].trim();
                } else if (line.startsWith("Rating:")) {
                    rating = Integer.parseInt(line.split(":")[1].trim());
                } else if (line.startsWith("Comments:")) {
                    comments = line.split(":")[1].trim();
                }
            }
            if (name != null) {
                feedbackList.add(new Feedback(name, rating, comments));
            }
        } catch (IOException e) {
            System.out.println("An error occurred while loading the feedbacks.");
        }
    }

    public List<Feedback> getAllFeedbacks() {
        return feedbackList;
    }
}

public class FeedbackSystemGUI {
    private JFrame frame;
    private FeedbackManager feedbackManager;
    private JTextArea feedbackArea;

    public FeedbackSystemGUI() {
        feedbackManager = new FeedbackManager();
        createUI();
    }

    private void createUI() {
        frame = new JFrame("Feedback Collection System");
        frame.setSize(400, 300);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setLocationRelativeTo(null);

        JPanel panel = new JPanel();
        panel.setLayout(new BorderLayout());

        JButton submitButton = new JButton("Submit Feedback");
        JButton viewButton = new JButton("View Feedbacks");
        feedbackArea = new JTextArea();
        feedbackArea.setEditable(false);

        submitButton.addActionListener(e -> submitFeedback());
        viewButton.addActionListener(e -> viewFeedbacks());

        JPanel buttonPanel = new JPanel();
        buttonPanel.add(submitButton);
        buttonPanel.add(viewButton);

        panel.add(buttonPanel, BorderLayout.NORTH);
        panel.add(new JScrollPane(feedbackArea), BorderLayout.CENTER);

        frame.add(panel);
        frame.setVisible(true);
    }

    private void submitFeedback() {
        JTextField nameField = new JTextField();
        JTextField ratingField = new JTextField();
        JTextArea commentsArea = new JTextArea(5, 20);

        JPanel inputPanel = new JPanel(new GridLayout(0, 1));
        inputPanel.add(new JLabel("Enter your name:"));
        inputPanel.add(nameField);
        inputPanel.add(new JLabel("Rate the product (1-5):"));
        inputPanel.add(ratingField);
        inputPanel.add(new JLabel("Enter your comments:"));
        inputPanel.add(new JScrollPane(commentsArea));

        int option = JOptionPane.showConfirmDialog(frame, inputPanel, "Submit Feedback",
                JOptionPane.OK_CANCEL_OPTION);

        if (option == JOptionPane.OK_OPTION) {
            String name = nameField.getText();
            String ratingStr = ratingField.getText();
            String comments = commentsArea.getText();

            try {
                int rating = Integer.parseInt(ratingStr);
                if (rating < 1 || rating > 5) {
                    JOptionPane.showMessageDialog(frame, "Please enter a rating between 1 and 5.",
                            "Invalid Rating", JOptionPane.ERROR_MESSAGE);
                    return;
                }

                Feedback feedback = new Feedback(name, rating, comments);
                feedbackManager.addFeedback(feedback);
                JOptionPane.showMessageDialog(frame, "Thank you for your feedback!",
                        "Feedback Submitted", JOptionPane.INFORMATION_MESSAGE);
            } catch (NumberFormatException ex) {
                JOptionPane.showMessageDialog(frame, "Invalid rating! Please enter a number between 1 and 5.",
                        "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
    }

    private void viewFeedbacks() {
        List<Feedback> feedbackList = feedbackManager.getAllFeedbacks();
        if (feedbackList.isEmpty()) {
            JOptionPane.showMessageDialog(frame, "No feedbacks available.",
                    "No Feedbacks", JOptionPane.INFORMATION_MESSAGE);
        } else {
            StringBuilder feedbackText = new StringBuilder();
            for (Feedback feedback : feedbackList) {
                feedbackText.append(feedback.toString()).append("\n");
            }
            feedbackArea.setText(feedbackText.toString());
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(FeedbackSystemGUI::new);
    }
}
