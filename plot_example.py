import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import io

# Arguments for the memory_latency program
max_size = 33554432  # 32 MiB
factor = 2
repeat = 10

# Run the program and capture output
result = subprocess.run(
    ['./memory_latency', str(max_size), str(factor), str(repeat)],
    capture_output=True,
    text=True
)

# Check for errors
if result.returncode != 0:
    print("Error running memory_latency:", result.stderr)
    exit(1)

# Parse the output
output = result.stdout.strip()
data = pd.read_csv(io.StringIO(output), header=None, names=['mem_size', 'offset', 'offset_sequential'])

# Plotting
plt.plot(data['mem_size'], data['offset'], label="Random access")
plt.plot(data['mem_size'], data['offset_sequential'], label="Sequential access")
plt.xscale('log')
plt.yscale('log')

# Optional: Annotate cache sizes if known (e.g., 32 KiB, 256 KiB, 8 MiB)
l1_size = 32 * 1024
l2_size = 256 * 1024
l3_size = 8 * 1024 * 1024
plt.axvline(x=l1_size, label="L1 (32 KiB)", c='r')
plt.axvline(x=l2_size, label="L2 (256 KiB)", c='g')
plt.axvline(x=l3_size, label="L3 (8 MiB)", c='brown')

plt.legend()
plt.title("Latency as a function of array size")
plt.ylabel("Latency (ns, log scale)")
plt.xlabel("Bytes allocated (log scale)")
plt.tight_layout()
plt.savefig("latency_plot.png")
plt.show()
