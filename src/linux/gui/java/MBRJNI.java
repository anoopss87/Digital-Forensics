public class MBRJNI {
	private MBR mbrEntry;
	private native boolean loadMBR(String disk);
	private native String getPartitionInfo(int partition);

	static {
		System.loadLibrary("shared");
	}

	public MBRJNI() throws Exception {
		boolean b = loadMBR("/dev/sda");
		if(!b) {
			throw new Exception("MBR could not be loaded.");
		}
		mbrEntry = new MBR();
		for(int i=1; i<=Integer.valueOf(getPartitionInfo(0)); i++) {
			mbrEntry.setPartitionEntry(i, getPartitionInfo(i));
		}
	}

	public MBRJNI(String disk) throws Exception {
		boolean b = loadMBR(disk);
		if(!b) {
			throw new Exception("MBR could not be loaded.");
		}
		mbrEntry = new MBR();
		for(int i=1; i<=Integer.valueOf(getPartitionInfo(0)); i++) {
			mbrEntry.setPartitionEntry(i, getPartitionInfo(i));
		}
	}

	public int getTotalPartitions() {
		return Integer.valueOf(getPartitionInfo(0));
	}
	
	public String getPartitionEntry(int i) {
		return mbrEntry.getPartitionEntry(i);
	}

	private class MBR {
		private class PartitionEntry {
			private boolean bootable;
			private CHSAddress firstSector;
			private CHSAddress lastSector;
			private String partitionType;
			private long numSectors;
			private long logicalAddress;

			public PartitionEntry(String partition) {
				String[] args = partition.split(" ");
				int start = 0;
				if(args!= null && args.length >= 10) {
					start = Integer.valueOf(args[0]);
				}
				bootable = ((start & 1<<7)!=0);
				firstSector = new CHSAddress(Integer.valueOf(args[1]), Integer.valueOf(args[2]), Integer.valueOf(args[3]));
				lastSector = new CHSAddress(Integer.valueOf(args[5]), Integer.valueOf(args[6]), Integer.valueOf(args[7]));
				partitionType = args[4];
				logicalAddress = Long.valueOf(args[8]);
				numSectors = Long.valueOf(args[9]);
			}

			public boolean isBootable() {
				return bootable;
			}

			public CHSAddress getFirstSector() {
				return firstSector;
			}

			public CHSAddress getLastSector() {
				return lastSector;
			}

			public String getPartitionType() {
				return partitionType;
			}

			public long getNumSectors() {
				return numSectors;
			}

			public long getPartitionSize() {
				return 512*numSectors;
			}

			public long getLogicalAddress() {
				return logicalAddress;
			}

			@Override
			public String toString() {
				String returnValue1 = String.valueOf(isBootable()) + " " + getFirstSector().toString() + " " + getLastSector().toString() + " " + getPartitionType();
				String returnValue2 = " " + String.valueOf(getLogicalAddress()) + " " + String.valueOf(getNumSectors()) + " " + String.valueOf(getPartitionSize());
				return returnValue1+returnValue2;
			}
		}

		private class CHSAddress {
			private int head;
			private int sector;
			private int cylinder;
			public CHSAddress(int h, int s, int c) {
				head = h;
				sector = s;
				cylinder = c;
			}
			
			@Override
			public String toString() {
				return ""+head+" "+sector+" "+cylinder;
			}
		}

//		private String bootStrapCode;	//TODO: to be handled later
		private PartitionEntry partition1;
		private PartitionEntry partition2;
		private PartitionEntry partition3;
		private PartitionEntry partition4;

		public void setPartitionEntry(int i, String partition) {
			switch(i) {
			case 1:
				partition1 = new PartitionEntry(partition);
				break;
			case 2:
				partition2 = new PartitionEntry(partition);
				break;
			case 3:
				partition3 = new PartitionEntry(partition);
				break;
			case 4:
				partition4 = new PartitionEntry(partition);
				break;
			default:
				break;
			}
		}
		
		public String getPartitionEntry(int i) {
			switch(i) {
			case 1:
				if(partition1 != null) {
					return partition1.toString();
				} else {
					return "";
				}
			case 2:
				if(partition2 != null) {
					return partition2.toString();
				} else {
					return "";
				}
			case 3:
				if(partition3 != null) {
					return partition3.toString();
				} else {
					return "";
				}
			case 4:
				if(partition4 != null) {
					return partition4.toString();
				} else {
					return "";
				}
			default:
				return "";
			}
		}
	}
}
