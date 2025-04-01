
import pandas as pd
import matplotlib.pyplot as plt

# Load the file with strict parsing
df = pd.read_csv("D:/ROOT_000.CSV", dtype=str)
# Parse timestamp if needed
# df["timestamp"] = pd.to_datetime(df["timestamp"], format="%H-%M-%S.%f", errors="coerce")
df["cableLength[cm]"] = pd.to_numeric(df["cableLength[cm]"].str.strip(), errors="coerce")
df["weight[N]"] = pd.to_numeric(df["weight[N]"].str.strip(), errors="coerce")

# Create the plot
fig, ax1 = plt.subplots(figsize=(12, 6))

# Left Y-axis: Force
ax1.plot(df["timestamp"], df["weight[N]"], color="blue", label="Force [N]")
ax1.set_ylabel("Force [N]", color="blue")
ax1.tick_params(axis='y', labelcolor="blue")

# Right Y-axis: Cable length
ax2 = ax1.twinx()
ax2.plot(df["timestamp"], df["cableLength[cm]"], color="red", label="Cable Length [cm]")
ax2.set_ylabel("Cable Length [cm]", color="red")
ax2.tick_params(axis='y', labelcolor="red")

# X-axis and title
ax1.set_xlabel("Time")

# Show only every 20th tick on the X-axis
ax1.set_xticks(df["timestamp"][::40])
ax1.tick_params(axis='x', rotation=45)

# Synchronize both Y-axes to share the same 0 baseline
min_y = min(ax1.get_ylim()[0], ax2.get_ylim()[0])
max_y = max(ax1.get_ylim()[1], ax2.get_ylim()[1])
ax1.set_ylim(min_y, max_y)
ax2.set_ylim(min_y, max_y)

plt.title("RootDataLogger: Force and Cable Length Over Time")
plt.tight_layout()
plt.show()
