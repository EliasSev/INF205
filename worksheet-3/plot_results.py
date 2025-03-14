import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import seaborn as sns
import numpy as np


## FILE PATHS
RESULTS_FILE = "simulation-results-5.csv"
FIGURE_1 = "low-densities-5.png"         # figure 1 and 2 in pdf
FIGURE_2 = "low-densities-simple-5.png"  # used to generate FIGURE_3
FIGURE_3 = "boundaries-5.png"            # figure 3 in pdf
FIGURE_4 = "comparisons-5.png"           # figure 4 in pdf


## FIGURE 1
results = pd.read_csv(RESULTS_FILE)
results_hm = results.pivot(index="density", columns="box_size", values="speedup")

plt.figure(figsize=(10, 8))
norm = mcolors.TwoSlopeNorm(vmin=0, vcenter=1, vmax=2)
ax = sns.heatmap(results_hm, cmap="bwr_r", annot=True, fmt=".1f", cbar=True, norm=norm, annot_kws={"size": 6})

# Labels and title
plt.xlabel("Box Size (a)")
plt.ylabel("Density ($\\rho$)")
plt.title("Speed-up of using linked-cell versus iteration")
plt.savefig(FIGURE_1)


## FIGURE 2
plt.figure(figsize=(8, 8))
norm = mcolors.TwoSlopeNorm(vmin=0, vcenter=1, vmax=2)
ax = sns.heatmap(results_hm, cmap="bwr_r", annot=True, fmt=".1f", cbar=False, norm=norm, annot_kws={"size": 6})

# remove the axis labels and ticks
ax.set_xticks([])
ax.set_yticks([])
ax.set_xlabel('')
ax.set_ylabel('') 
ax.invert_yaxis()
ax.set_title('')

plt.savefig(FIGURE_2, bbox_inches='tight', pad_inches=0)


## FIGURE 3
img = plt.imread(FIGURE_2)  # read in fig 2 and plot on top of it
height, width, _ = img.shape

# create the figure and axis
fig, ax = plt.subplots(figsize=(8, 8))

# xlim and ylim
xlim = (8-0.5, 45+0.5)
ylim = (0.003-0.0005, 0.025+0.0005)

f1 = lambda x: 0.0018 * x - 0.0114
f2 = lambda x: -0.00063 * x + 0.0201
f3 = lambda x: -0.0002* x + 0.011

x = np.linspace(xlim[0], 13, 100)
ax.plot(x, f1(x), c='lime', ls='-', lw=2, label="$f_1(x)$")
x = np.linspace(13, 21, 100)
ax.plot(x, f2(x), c='lime', ls='--', lw=2, label="$f_2(x)$")
x = np.linspace(21, 42, 100)
ax.plot(x, f3(x), c='lime', ls='-.', lw=2, label="$f_3(x)$")

# more ticks
ax.set_xticks(np.linspace(8, 45, 38))  # Add 10 ticks along the x-axis
ax.set_yticks(np.arange(0.003, 0.026, 0.001))  # Add 10 ticks along the y-axis
plt.xticks(rotation=90)  # Rotate x-axis tick labels
plt.yticks(rotation=0)  # Rotate y-axis tick labels

# display the image with the desired extent
extent = [xlim[0], xlim[1], ylim[0], ylim[1]]
ax.imshow(img, extent=extent, origin="upper")
ax.set_aspect('auto')
plt.legend()
plt.xlabel("Box Size (a)")
plt.ylabel("Density ($\\rho$)")
plt.savefig(FIGURE_3)


## FIGURE 4
results["N"] = np.floor(results["box_size"]**3 * results["density"])
results["n_cells"] = np.floor(results["box_size"] / 2.5)
results["m/c"] = results["N"] / results["n_cells"]**3
results["m/c"].max(), results["m/c"].min()

hm1 = results.pivot(index="density", columns="box_size", values="N")
hm2 = results.pivot(index="density", columns="box_size", values="m/c")

fig, axes = plt.subplots(1, 2, figsize=(11, 5), sharey=True)
norm1 = mcolors.TwoSlopeNorm(vmin=1, vcenter=50, vmax=300)
ax1 = sns.heatmap(hm1, cmap="bwr_r", annot=False, fmt=".1f", cbar=True, norm=norm1, annot_kws={"size": 6}, ax=axes[0])
axes[0].set_title("Number of molecules")

# heatmap for m/c
norm2 = mcolors.TwoSlopeNorm(vmin=0, vcenter=0.1, vmax=0.5)
ax2 = sns.heatmap(hm2, cmap="bwr_r", annot=False, fmt=".1f", cbar=True, norm=norm2, annot_kws={"size": 6}, ax=axes[1])
axes[1].set_title("Molecules per cell")

# adjust colorbars
cbar1 = ax1.collections[0].colorbar
ticks1 = [1, 25, 50, 100, 150, 200, 250, 300] # hm1
cbar1.set_ticks(ticks1)
cbar1.set_ticklabels(ticks1)

cbar2 = ax2.collections[0].colorbar
ticks2 = np.round(np.linspace(0, 0.5, 10), 2)  # hm2
cbar2.set_ticks(ticks2)
cbar2.set_ticklabels(ticks2)

axes[0].set_ylabel("Density ($\\rho$)")
for ax in axes:
    ax.set_xlabel("Box Size (a)")

plt.tight_layout()
plt.savefig(FIGURE_4)
