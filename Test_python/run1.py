import numpy as np
import matplotlib.pyplot as plt # type = ignore
import matplotlib.animation as animation # type = ignore

# Grid size
N = 100

# Random initial state
grid = np.random.choice([0, 1], size=(N, N))

def update(frameNum, img, grid, N):
    newGrid = grid.copy()
    for i in range(N):
        for j in range(N):
            # Count neighbors
            neighbors = (grid[i, (j-1)%N] + grid[i, (j+1)%N] +
                         grid[(i-1)%N, j] + grid[(i+1)%N, j] +
                         grid[(i-1)%N, (j-1)%N] + grid[(i-1)%N, (j+1)%N] +
                         grid[(i+1)%N, (j-1)%N] + grid[(i+1)%N, (j+1)%N])
            # Rules
            if grid[i, j] == 1 and (neighbors < 2 or neighbors > 3):
                newGrid[i, j] = 0
            elif grid[i, j] == 0 and neighbors == 3:
                newGrid[i, j] = 1
    img.set_data(newGrid)
    grid[:] = newGrid[:]
    return img,

# Animation
fig, ax = plt.subplots()
img = ax.imshow(grid, interpolation='nearest', cmap='binary')
ani = animation.FuncAnimation(fig, update, fargs=(img, grid, N), interval=100, save_count=50)
plt.show()
