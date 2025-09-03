//* Example of Stroke in JAVA *
// by Luciano da Silva Ribas
// license under GNU General Public License

import java.awt.*;
import java.awt.event.*;
import java.math.*;

public class Paint extends Frame implements WindowListener, MouseListener,MouseMotionListener, ActionListener {
	public static Canvas c;
	public static Graphics g;
	public static int xp;
	public static int yp;
	public Color cor = Color.black;
	public Stroke MyStroke;

	public static int figura = 1;
	public static int rect_lx = 50;
	public static int rect_ly = 50;
	public static int circ_s = 100;

	public static void main(String args[]){
		Paint i = new Paint("Paint");
	}

	public Paint(String t){
		super(t);
		this.setSize(500,500);
		this.setLocation(100,100);
		this.setVisible(true);

		MyStroke = new Stroke();

		MenuBar mbar = new MenuBar();
		Menu menuAbout = new Menu("About");
		MenuItem miAbout = new MenuItem("About..");
		miAbout.addActionListener(this);
      mbar.add(menuAbout);
      menuAbout.add(miAbout);
		this.setMenuBar(mbar);

		c = new Canvas();
		this.add(c);

		c.addMouseListener(this);
		c.addMouseMotionListener(this);

		this.addWindowListener(this);
		this.show();

      System.out.println("\nLASD/CEFETPR\nTMN Project - Telecommunications Management Network\n\nAuthor: Luciano da Silva Ribas  ribas@lasd.cefetpr.br\n");
		System.out.println("Strokes actived.");
		System.out.println("\t1  2  3\n\t4  5  6\n\t7  8  9\n");
		System.out.println("to change color:");
		System.out.println("\t456 - Black");
		System.out.println("\t654 - Blue");
		System.out.println("\t258 - Yellow");
		System.out.println("\t852 - Red\n");
    	System.out.println("to change draw:");
		System.out.println("\t78963 - Oval");
		System.out.println("\t14789 - Rectangle\n");
  	   System.out.println("to change size:");
		System.out.println("\t357 - Zoom-in");
		System.out.println("\t753 - Zoom-out\n");

		System.out.println("detected:");

		Draw();
	}


	public void Draw() {
	
if( figura==1) {
			g=c.getGraphics();
			g.setColor(Color.white);
			g.fillRect(0,0,500,500);
			g.setColor(cor);
			g.fillOval(200,200,circ_s,circ_s);
			}

	if( figura==2) {
			g=c.getGraphics();
			g.setColor(Color.white);
			g.fillRect(0,0,500,500);
			g.setColor(cor);
			g.fillRect(200-rect_lx,200-rect_ly,200+rect_lx,200+rect_ly);
			}
	}

	public void actionPerformed( ActionEvent a){
		String comand=a.getActionCommand();
		if(comand.equals("About.."))
      System.out.println("\nLASD/CEFETPR\nTMN Project - Telecommunications Management Network\n\nAuthor: Luciano da Silva Ribas  ribas@lasd.cefetpr.br\n");
	}

	public void mousePressed(MouseEvent m){
		xp = m.getX();
		yp = m.getY();
		g=c.getGraphics();
		g.setColor(cor);
		g.drawLine(xp,yp,xp,yp);	
	}

	public void mouseReleased(MouseEvent m){
		String Result;
		Result = MyStroke.stroke_trans();
		System.out.print("----> ["+Result+"] .. ");

		if (Result.equals("456")) {
		  cor = Color.black;
		  System.out.println("color black.");
		} else
		if (Result.equals("654")) {
		  cor = Color.blue;
		  System.out.println("color blue."); 
		} else 
		if (Result.equals("258")) {
		  cor = Color.yellow;
		  System.out.println("color yellow.");
		} else 
		if (Result.equals("852")) {
		  cor = Color.red;
		  System.out.println("color red.");
		} else 
		if (Result.equals("78963")) {
		  figura = 1;
		  System.out.println("draw circle.");
		} else
		if (Result.equals("14789")) {
		  figura = 2;
		  System.out.println("draw rectangle.");
		} else
		if (Result.equals("357")) {
		  rect_lx *= 2;
		  rect_ly *= 2;
		  circ_s *= 2;
		  System.out.println("ZOOM IN.");
		} else
		if (Result.equals("753")) {
		  rect_lx = (int) rect_lx/2;
		  rect_ly = (int) rect_ly/2;
		  circ_s = (int) circ_s/2;
		  System.out.println("ZOOM OUT.");
		} else {
		System.out.println("stroke not recognizable.");}

		Draw();
	}

	public void mouseEntered(MouseEvent m){}
	public void mouseExited(MouseEvent m){}
	public void mouseClicked(MouseEvent m){}

	public void mouseDragged(MouseEvent m){
		int x=m.getX();
		int y=m.getY();
		g=c.getGraphics();
		g.setColor(Color.black);
		g.drawLine(x,y,xp,yp);
		xp=x;
		yp=y;
		MyStroke.stroke_record( x, y );
	}

	public void mouseMoved(MouseEvent m){}

	public void windowClosing (WindowEvent e){
		System.exit(0);}
	public void windowOpened (WindowEvent e){}
	public void windowIconified (WindowEvent e){}
	public void windowDeiconified (WindowEvent e){}
 	public void windowClosed (WindowEvent e){}
 	public void windowActivated (WindowEvent e){}
	public void windowDeactivated (WindowEvent e){}
}

