public class Partition {
	private static MBRJNI mbrData;

	static {
		try {
			mbrData = new MBRJNI();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static MBRJNI updateMBRData(String disk) {
		try {
			mbrData = new MBRJNI(disk);
		} catch (Exception e) {
			e.printStackTrace();
			mbrData = null;
		}
		return mbrData;
	}

	public static int getTotalPartitions() {
		return mbrData.getTotalPartitions();
	}

	public static Partition getPartition(String disk, int i) {
		Partition p1 = null;
		if(i > 0 && i < 5) {
			p1 = new Partition(mbrData.getPartitionEntry(i));
			try{
				SuperBlockJNI group = new SuperBlockJNI(disk+i);
				p1.setPhysicalSize(group.getDiskSize());
				p1.setValidExt(true);
			} catch(Exception ex) {
				p1.setValidExt(false);
			}
		}
		return p1;
	}

	public String getPartitionType() {
		int type = Integer.valueOf(partitionType, 16);
		if(type == 131) {
			return "Linux";
		} else if(type == 39) {
			return "Windows Hidden Partition";
		} else if(type == 7) {
			return "Windows NT";
		} else {
			return "Unknown";
		}
	}

	private boolean active;
	private String firstSector;
	private String partitionType;
	private String lastSector;
	private long firstSectorLBA;
	private long totalSectors;
	private long partitionSize;
	private boolean validity;
	private long physicalSize;

	private void init() {
		active = false;
		firstSector = "";
		partitionType = "";
		lastSector = "";
		firstSectorLBA = 0;
		totalSectors = 0;
		partitionSize = 0;
		validity = false;
		physicalSize = 0;
	}

	public void setValidExt(boolean b) {
		validity = b;
	}

	private Partition() {
		init();
	}

	public void setPhysicalSize(long size) {
		physicalSize = size;
	}
	
	public long getPhysicalSize() {
		return physicalSize;
	}

	private Partition(String str) {
		init();
		if(str == "") {
			return;
		}
		String[] args = str.split(" ");
		if(args.length < 10) {
			return;
		}
		active = Boolean.valueOf(args[0]);
		firstSector = args[1] + " " + args[2] + " " + args[3];
		lastSector = args[4] + " " + args[5] + " " + args[6];
		partitionType = args[7];
		firstSectorLBA = Long.valueOf(args[8]);
		totalSectors = Long.valueOf(args[9]);
		partitionSize = Long.valueOf(args[10]);
	}

	public String readableSize(long a) {
		StringBuilder sb = new StringBuilder();
		while(a>999) {
			sb.insert(0, String.format("%03d", a%1000));
			sb.insert(0, ',');
			a = a/1000;
		}
		sb.insert(0, a);
		sb.append(" Bytes");
		return sb.toString();
	}

	@Override
	public String toString() {
		String str = "Address of First Sector: "+firstSector+"\nPartition Type: "+partitionType
				+"\nAddress of Last Sector: "+lastSector+"\nLogical Block Address of First Sector: 0x"+String.format("%08x", /*Long.toHexString*/(firstSectorLBA))
				+"\nTotal Sectors: "+totalSectors+"\nPartition Size: "+readableSize(partitionSize);
		String act;
		if(active) {
			act = "Active: Yes\n";
		} else {
			act = "Active: No\n";
		}
		str = act+str;
		return str;
	}

	public boolean isValidExt() {
		return validity;
	}
}
