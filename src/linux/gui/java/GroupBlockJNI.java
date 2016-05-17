import java.util.HashMap;

public class GroupBlockJNI {
	private native int getTotalBlockGroups(String disk);
	private native int init(String disk);
	private native String getBlockGroupInfo(int blockGroupNumber);
	private native String getBlockInfo(String disk, int blockGroupNumber);
	
	private int totalBlockGroups = 0;
	private int gBlockSize = 0;
	private String currentDisk;
	private HashMap<Integer, String> blockBitmap;

	static {
		System.loadLibrary("shared");
	}

	public GroupBlockJNI(){
	}

	public GroupBlockJNI(String disk){
		totalBlockGroups = getTotalBlockGroups(disk);
		currentDisk = disk;
		gBlockSize = init(disk);
		System.out.println(gBlockSize);
		blockBitmap = new HashMap<Integer, String>();
	}

	public int numberOfBlockGroups() {
		return totalBlockGroups;
	}

	public String getGroupInfo(int groupNumber) {
		String info = getBlockGroupInfo(groupNumber);
		String[] information = info.split(" ");
		info = "Block bitmap = Block "+Long.valueOf(information[0],16)+"\n"
		+ "Inode bitmap = Block "+Long.valueOf(information[1],16)+"\n"
		+ "Inode table = Block "+Long.valueOf(information[2],16)+"\n"
		+ "Free inodes count = "+Integer.valueOf(information[3], 16)+"\n"
		+ "Free blocks count = "+Integer.valueOf(information[4], 16)+"\n"
		+ "Used Directories count = "+Integer.valueOf(information[5], 16);
		return info;
	}

	public long[] getPlainGroupInfo(int groupNumber) {
		String[] info = getBlockGroupInfo(groupNumber).split(" ");
		long[] information = new long[info.length];
		for(int i=0; i<info.length; i++) {
			information[i] = Long.valueOf(info[i], 16);
		}
		return information;
	}
	
	public String getBlockColorCoding(int groupNumber) {
		if(blockBitmap == null)
			blockBitmap = new HashMap<Integer, String>();
		if(!blockBitmap.containsKey(groupNumber)) {
			String code = getBlockInfo(currentDisk, groupNumber);
			blockBitmap.put(groupNumber, code);
		}
		return blockBitmap.get(groupNumber);
	}
}
