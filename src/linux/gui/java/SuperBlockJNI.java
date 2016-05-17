public class SuperBlockJNI {
	private native boolean init(String disk);
	private native int getNumberofBlocks();
	private native long getPartitionSize();
	private native String getPartitionInfo();
	private native int getInodeTableSize();
	private native int getReservedGDTBlocks();
	private native int getBlockSize();

	static {
		System.loadLibrary("shared");
	}

	public SuperBlockJNI() {
		init("/dev/sda4");
	}

	public SuperBlockJNI(String disk) throws Exception {
		System.out.println("Init "+disk);
		if(init(disk))
			return;
		throw new Exception();
	}

	public int[] getSuperblockData() {
		System.out.println("data");
		int[] blockData = new int[4];
		blockData[0] = getNumberofBlocks();
		blockData[1] = getInodeTableSize();
		blockData[2] = getReservedGDTBlocks();
		blockData[3] = getBlockSize();
		return blockData;
	}
	public String getBlockInfo(int block) {
		System.out.println("block info");
		return "Block";
	}

	public String partitionInfo() {
		System.out.println("partition info");
		return getPartitionInfo();
	}

	public long getDiskSize() {
		System.out.println("disk size");
		return getPartitionSize();
	}
}
