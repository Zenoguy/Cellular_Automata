package Test_Java;

import javax.swing.*;
import java.awt.*;
import java.util.Random;

public class LifeGUI extends JPanel {
    static final int N = 50;        // Grid size
    static final int CELL_SIZE = 20; // Pixel size of each cell
    static final int DELAY = 100;    // Milliseconds between frames

    int[][] grid = new int[N][N];
    int[][] newGrid = new int[N][N];
    Random rand = new Random();

    public LifeGUI() {
        // Random initialization
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                grid[i][j] = rand.nextInt(2);
            }
        }

        // Timer to update simulation
        Timer timer = new Timer(DELAY, e -> {
            step();
            repaint();
        });
        timer.start();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 1) {
                    g.setColor(Color.BLACK);
                } else {
                    g.setColor(Color.WHITE);
                }
                g.fillRect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                g.setColor(Color.LIGHT_GRAY); // draw grid lines
                g.drawRect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            }
        }
    }

    private void step() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int neighbors = countNeighbors(i, j);
                if (grid[i][j] == 1) {
                    newGrid[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
                } else {
                    newGrid[i][j] = (neighbors == 3) ? 1 : 0;
                }
            }
        }

        // Swap grids
        for (int i = 0; i < N; i++) {
            System.arraycopy(newGrid[i], 0, grid[i], 0, N);
        }
    }

    private int countNeighbors(int x, int y) {
        int count = 0;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                int nx = (x + dx + N) % N;
                int ny = (y + dy + N) % N;
                count += grid[nx][ny];
            }
        }
        return count;
    }

    public static void main(String[] args) {
        JFrame frame = new JFrame("Conway's Game of Life");
        LifeGUI life = new LifeGUI();
        frame.add(life);
        frame.setSize(N * CELL_SIZE + 16, N * CELL_SIZE + 39); // adjust for window borders
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}
