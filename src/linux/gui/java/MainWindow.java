import java.awt.*;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.StyledDocument;

import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.awt.event.ActionEvent;

public class MainWindow {
	/**
	 * Launch the application.
	 */
	private JTextPane gDetailsText;
	private JButton gButtonZoomOut;
	private JButton gButtonValidate;
	private JProgressBar gProgressBar;
	private JPanel gScrollPanel;
	private int gCurrentLevel;
	private int currentPartition = 0;
	private int currentBlockGroup = 0;
	private String currentDisk = "/dev/sda";
	private String colorCoding;
	private Color[] colorCode = {null, Color.ORANGE, Color.YELLOW, Color.BLUE, Color.GREEN, Color.WHITE, Color.RED};
	private static final int ROOT_LEVEL = 1;
	private boolean isValid = false;
	private Task task;
	private long[] info;
	private Container content;

	class Task extends SwingWorker<Void, Void> {
		/*
		 * Main task. Executed in background thread.
		 * TODO: Use publish to give progress information to user.
		 */
		@Override
		public Void doInBackground() {
			gScrollPanel.setVisible(false);
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			gScrollPanel.removeAll();
			gScrollPanel.repaint();
			gScrollPanel.revalidate();
			switch(gCurrentLevel) {
			case 1:
				// create boot block
				createBootBlock(gScrollPanel);
				if(gButtonZoomOut != null) {
					gButtonZoomOut.setEnabled(false);
				}
				break;
			case 2:
				// create block groups
				createBlockGroup(gScrollPanel);
				if(gButtonZoomOut != null) {
					gButtonZoomOut.setEnabled(true);
				}
				break;
			case 3:
				// create blocks
				createBlockPanel(gScrollPanel);
				if(gButtonZoomOut != null) {
					gButtonZoomOut.setEnabled(true);
				}
				break;
			default:
				break;
			}
			gScrollPanel.setVisible(true);
			gScrollPanel.revalidate();
			return null;
		}

		/*
		 * Executed in event dispatching thread
		 */
		@Override
		public void done() {
			Toolkit.getDefaultToolkit().beep();
			//            startButton.setEnabled(true);
			//            setCursor(null); //turn off the wait cursor
			gProgressBar.setVisible(false);
		}
	}

	private void updateText(String message) {
		if(gDetailsText != null) {
			gDetailsText.setText(message);
		}
	}

	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					new MainWindow();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public MainWindow() {
		try {
			setLibraryPath();
		} catch (NoSuchFieldException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		gCurrentLevel = ROOT_LEVEL;
		createAndShowGUI();
	}

	private ArrayList<String> getDiskInformation() {
		// get disk information
		ArrayList<String> diskArray = new ArrayList<String>();
		String disk = "/dev/sd";
		for(int i=0; i<26; i++) {
			char append = (char)((int)'a'+i);	// checks for all: sda to sdz.
			File f = new File(disk+append);
			if(f.exists()) {
				diskArray.add(disk+append);
			}
		}
		return diskArray;
	}

	private void createAndShowGUI() {
		JFrame frame = new JFrame("Digital Forensics");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		content = frame.getContentPane();

		Choice choice = new Choice();
		ArrayList<String> diskArray = getDiskInformation();
		for(int i=0; i<diskArray.size(); i++) {
			choice.add(diskArray.get(i));
		}
		choice.addItemListener(new ItemListener() {
			@Override
			public void itemStateChanged(ItemEvent e) {
				currentDisk = e.getItem().toString();
				Partition.updateMBRData(currentDisk);
				updateLevel(ROOT_LEVEL);
			}
		});
		content.add(choice, BorderLayout.NORTH);

		gScrollPanel = new JPanel();
		JScrollPane pane = new JScrollPane(gScrollPanel);
		content.add(pane, BorderLayout.CENTER);

		JPanel detailsPanel = new JPanel(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 0;
		gProgressBar = new JProgressBar(0,100);
		//		gProgressBar.setStringPainted(true);
		gProgressBar.setVisible(false);
		detailsPanel.add(gProgressBar,c);
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.gridy = 1;
		c.weightx = 1.0;
		c.weighty = 1.0;
		gDetailsText = new JTextPane();
		gDetailsText.setEditable(false);
		gDetailsText.setBackground(new Color(255,255,255,255));
		gDetailsText.setBorder(new TitledBorder(null, "Details", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		gDetailsText.setText("");
		gDetailsText.setPreferredSize(new Dimension(700,200));
		detailsPanel.add(gDetailsText, c);
		//		content.add(gDetailsText, BorderLayout.SOUTH);
		content.add(detailsPanel, BorderLayout.SOUTH);

		JPanel buttonArea = new JPanel(new GridLayout(2,1));
		gButtonZoomOut = new JButton("Up one level");
		gButtonZoomOut.setEnabled(false);
		gButtonZoomOut.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				updateLevel(gCurrentLevel-1);
			}
		});
		gButtonValidate = new JButton("Validate");
		gButtonValidate.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent event) {
				if(gButtonValidate.getText() == "Validate") {
					System.out.println("Validating partition "+currentPartition);
					// TODO: change according to gCurrentLevel
					if(isValid) {
						showPopup(content, "This is a valid EXT drive.");
						gButtonValidate.setText("Update");
					} else {
						showPopup(content, "This is not a valid EXT drive.");
					}
				} else {
					System.out.println("Updating partition "+currentPartition);
					// TODO: change according to gCurrentLevel
					if(isValid) {
						// TODO: update the values.
					}
					gButtonValidate.setText("Validate");
				}
			}
		});
		//		content.add(gButtonZoomOut, BorderLayout.EAST);
		buttonArea.add(gButtonZoomOut);
		buttonArea.add(gButtonValidate);
		content.add(buttonArea, BorderLayout.EAST);

		info = new long[6];

		updateLevel(ROOT_LEVEL);

		frame.setSize(800, 400);
		frame.setVisible(true);
	}

	private void resetValidate() {
		if(gButtonValidate == null)
			return;
		gButtonValidate.setEnabled(false);
		gButtonValidate.setText("Validate");
	}

	private void updateColorCodingIcon() {
		StyledDocument doc = gDetailsText.getStyledDocument();
		Style def = StyleContext.getDefaultStyleContext().getStyle(StyleContext.DEFAULT_STYLE);
		Style regular = doc.addStyle("regular", def);
		Style s = doc.addStyle("icon", regular);
		StyleConstants.setAlignment(s, StyleConstants.ALIGN_CENTER);
		ImageIcon icon = createImageIcon("images/colorCode.jpg", "Color Code Mapping");
		if(icon != null) {
			StyleConstants.setIcon(s,icon);
		}
		try {
			doc.insertString(doc.getLength(), " ", doc.getStyle("icon"));
		} catch (BadLocationException e1) {
			System.err.println("Couldn't insert styled text.");
		}
	}

	private ImageIcon createImageIcon(String path, String description) {
		java.net.URL imgURL = MainWindow.class.getResource(path);
		if (imgURL != null) {
			return new ImageIcon(imgURL, description);
		} else {
			System.err.println("Couldn't find file: " + path);
			return null;
		}
	}

	private void showPopup(Component panel, String msg) {
		JOptionPane.showMessageDialog(panel, msg);
	}

	private void updateLevel(int level) {
		resetValidate();
		gProgressBar.setIndeterminate(true);
		gProgressBar.setVisible(true);
		gCurrentLevel = level;
		task = new Task();
		task.execute();
	}

	private void createBootBlock(JPanel blockPanel) {
		Dimension buttonDimension = new Dimension(140,70);

		JButton mbrInfo = new JButton("MBR Data");
		mbrInfo.setPreferredSize(buttonDimension);
		mbrInfo.setBackground(Color.WHITE);
		mbrInfo.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				updateText("This is a placeholder for MBR data.");
			}
		});
		blockPanel.add(mbrInfo);

		for(int i=0; i<Partition.getTotalPartitions(); i++) {
			final int j = i+1;
			final Partition p1 = Partition.getPartition(currentDisk, j);
			String partitionType = p1.getPartitionType();
			JButton partition = new JButton("<html><center>"+partitionType+"</center></html>");
			partition.setBackground(Color.WHITE);
			partition.setPreferredSize(buttonDimension);
			partition.addMouseListener(new MouseAdapter() {
				@Override
				public void mouseClicked(MouseEvent e) {
					if(e.getClickCount() == 2) {
						if(p1.isValidExt()) {
							updateLevel(gCurrentLevel+1);
							gButtonZoomOut.setEnabled(true);
							resetValidate();
						} else {
							showPopup(content, "This drive is not a valid EXT partition.");
						}
					} else if(e.getClickCount() == 1) {
						resetValidate();
						currentPartition = j;
						isValid = p1.isValidExt();
						String valid;
						String physical = "";
						if(isValid) {
							valid = "a valid";
							physical = "\nThe physical size of the partition is "+p1.getPhysicalSize()+" bytes.";
						} else {
							valid = "not an";
						}
						gButtonValidate.setEnabled(true);
						updateText(p1.toString()+"\nThe current partition is "+valid+" EXT partition."+physical);
					}
				}
			});
			blockPanel.add(partition);
		}
	}

	private void updateGroupInfo(long[] plainGroupInfo) {
		for(int i=0;i<plainGroupInfo.length; i++) {
			info[i] = plainGroupInfo[i];
		}
	}

	private void createBlockGroup(JPanel blockPanel) {
		Dimension buttonDimension = new Dimension(140,70);
		String disk = currentDisk+currentPartition;
		final GroupBlockJNI group = new GroupBlockJNI(disk);
		//		SuperBlockJNI superblock = null;
		//		try {
		//			superblock = new SuperBlockJNI(disk);
		//		} catch(Exception ex) {
		//		}
		//		if(superblock == null)
		//			return;

		for(int i=0; i<group.numberOfBlockGroups(); i++) {
			final int j=i;
			JButton partition = new JButton("<html><center>"+"Block Group<br>"+j+"</center></html>");
			partition.setPreferredSize(buttonDimension);
			partition.addMouseListener(new MouseAdapter() {
				@Override
				public void mouseClicked(MouseEvent e) {
					if(e.getClickCount() == 2) {
						colorCoding = group.getBlockColorCoding(j);
						updateText("");
						updateColorCodingIcon();
						updateLevel(gCurrentLevel+1);
						gButtonZoomOut.setEnabled(true);
					} else if(e.getClickCount() == 1) {
						currentBlockGroup = j;
						updateGroupInfo(group.getPlainGroupInfo(j));
						updateText(group.getGroupInfo(j));
					}
				}
			});
			blockPanel.add(partition);
		}
	}

	private void setText(JButton button, String message) {
		//		button.setText("<html><center>"+message+"</center></html>");
		button.setText(message);
	}

	private void createBlockPanel(JPanel blockPanel) {
		Dimension buttonDimension = new Dimension(150,70);
		String disk = currentDisk+currentPartition;
		SuperBlockJNI group = null;
		try {
			group = new SuperBlockJNI(disk);
		} catch(Exception ex) {
			System.err.println("Vikram3 error detected.");
		}
		if(group == null) {
			System.err.println("Superblock not initialized. Returning early.");
			return;
		}
		int[] data = group.getSuperblockData();
		int totalBlocks = data[0];
		info[0] %= totalBlocks;
		info[1] %= totalBlocks;
		info[2] %= totalBlocks;
		if(data[3] == 1024) {
			for(int i=0; i<3; i++)
				info[i] -= 1;
		}
		String[] color = {"Used Block", "Null Block", "Address Block", "Text Block", "<html><center>Free Block with Junk Data</center></html>", "Metadata block"};

		for(int j=0; j<totalBlocks; j++) {
			int colorId = colorCoding.charAt(j);
			JButton partition = new JButton(color[colorId-1]);
			partition.setBackground(colorCode[6]);
			if(j>=info[2]+data[1]) {
				partition.setBackground(colorCode[colorId]);
			} else if(j>=1 && j<=info[0]-data[2]-1) {
				setText(partition, "<html><center>Block Group Descriptor</center></html>");
				// 1 to info[0]-data[2]-1 : group descriptor
			} else if(j>=info[0]-data[2] && j<= info[0]-1) {
				setText(partition, "<html><center>Reserved GDT Blocks</center></html>");
				// info[0]-data[2] till info[0]-1 : reserved GDT
			} else if(j>=info[2] && j<=info[2]+data[1]-1) {
				setText(partition,"<html><center>Inode Table Entries</center></html>");
				// info[2] till info[2]+data[1]-1 : inode table
			} else if(j == info[0]) {
				setText(partition,"Block Bitmap");
				// info[0] : block bitmap
			} else if(j == info[1]) {
				setText(partition,"Inode Bitmap");
				// info[1] : inode bitmap
			} else if(j==0) {
				if(currentBlockGroup == 0) {
					setText(partition, "Superblock");
				} else {
					setText(partition, "<html><center>Reserved Superblock</center></html>");
				}
			}
			//			partition.setText("<html><center>Metadata Block</center></html>");
			partition.setPreferredSize(buttonDimension);
			blockPanel.add(partition);
		}
	}

	private void setLibraryPath() throws NoSuchFieldException, SecurityException, IllegalArgumentException, IllegalAccessException {
		System.setProperty( "java.library.path", "." );
		 
		Field fieldSysPath = ClassLoader.class.getDeclaredField( "sys_paths" );
		fieldSysPath.setAccessible( true );
		fieldSysPath.set( null, null );
	}
}
