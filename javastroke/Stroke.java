
/* 
   Java Stroke - a Java stroke interface library
   Copyright (c) 1996,1997,1998,1999,2000  Mark F. Willey, ETLA Technical
   Copyright (c) 1999  Luciano da Silva Ribas, ribas@lasd.cefetpr.br
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software, including the rights to use, copy,
   modify, merge, publish, and distribute copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

		  This program is free software; you can redistribute it and/or
		  modify it under the terms of the GNU General Public License
		  as published by the Free Software Foundation; version 2.

		  This program is distributed in the hope that it will be useful,
		  but WITHOUT ANY WARRANTY; without even the implied warranty of
		  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		  GNU General Public License for more details.

		  You should have received a copy of the GNU General Public License
		  along with this program; if not, write to the Free Software
		  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
			  
   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.
   
   See the file "LICENSE" for a copy of the GNU GPL terms.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.
   
   Except as contained in this notice, the name of the author shall
   name be used in advertising or otherwise to promote the sale, use or
   other dealings in this Software without prior written authorization
   from the author.
   
   Non-GPL commercial use licenses are available - contact copyright holder.
   
   Author: Mark F. Willey  --  willey@etla.net
   http://www.etla.net/
   
   Java Port Author: Luciano da Silva Ribas  --  ribas@lasd.cefetpr.br

*/ 

// version in Java by Luciano da Silve Ribas

/** Classe de implementacao do Stroke. */
public class Stroke {

	/** maximo de pontos permitidos por sample*/
	static int MAX_POINTS=10000; 

	/** numero minimo de pontos por sample requerido*/
	static int MIN_POINTS=50; 

	/** maximo numero de posicoes por stroke*/
	static int MAX_SEQUENCE=20;

	/** threshold do axis*/
	static int SCALE_RATIO=4; 

	/** percentagem dos pontos adicionado a sequencia*/
	static double BIN_COUNT_PERCENT=0.07; 

	/** Inner Classe para armazenar os pontos do stroke.*/
	class s_point {
		int x;
		int y;
		s_point next;

		public s_point() {
			x = 0; y = 0;
		}

		public s_point(int a, int b) {
			x = a; y = b;
		}	
	}

	/** Variaveis de controle */
	int point_count = 0;
	int min_x = -1;
	int max_x = 10000;
	int min_y = -1;
	int max_y = 10000;

	/** Ponteiro para o inicio da lista de pontos */
	s_point point_list_head;

	/** Ponteiro para o fim da lista de pontos */
	s_point point_list_tail;

	/** Verifica se o ponto pertence ao contorno */
	int stroke_bin (s_point point_p, int bound_x_1, int bound_x_2, int bound_y_1, int bound_y_2) {
		int bin_num = 1; 
  		if (point_p.x > bound_x_1) bin_num += 1;
  		if (point_p.x > bound_x_2) bin_num += 1;
  		if (point_p.y > bound_y_1) bin_num += 3;
  		if (point_p.y > bound_y_2) bin_num += 3;
 	 	return bin_num;
	} 

	int stroke_init () {return 0;}

	String stroke_trans () {
		
			 int sequence_count = 0;
 		 	 int prev_bin = 0; 
			 int current_bin = 0;
			 int bin_count = 0;
			 int first_bin = 1;
			 int delta_x, delta_y;
			 int bound_x_1, bound_x_2;
			 int bound_y_1, bound_y_2;

			 String Result = new String("");
 
			 delta_x = max_x - min_x;
			 delta_y = max_y - min_y;
			 
			 bound_x_1 = min_x + (delta_x / 3); 
			 bound_x_2 = min_x + 2 * (delta_x / 3);
			 
			 bound_y_1 = min_y + (delta_y / 3); 
			 bound_y_2 = min_y + 2 * (delta_y / 3);
			 
			 if (delta_x > SCALE_RATIO * delta_y) {
				  bound_y_1 = (max_y + min_y - delta_x) / 2 + (delta_x / 3); 
				  bound_y_2 = (max_y + min_y - delta_x) / 2 + 2 * (delta_x / 3);
			 } else if (delta_y > SCALE_RATIO * delta_x) {
				  bound_x_1 = (max_x + min_x - delta_y) / 2 + (delta_y / 3); 
				  bound_x_2 = (max_x + min_x - delta_y) / 2 + 2 * (delta_y / 3);
			 }
			 
			 while (point_list_head != null) {
				
				current_bin = stroke_bin(point_list_head,bound_x_1, bound_x_2, bound_y_1, bound_y_2);
				prev_bin = (prev_bin == 0) ? current_bin : prev_bin;
				
				if (prev_bin == current_bin)
				  bin_count++;
				else {  
				  if ((bin_count > (point_count * BIN_COUNT_PERCENT)) || (first_bin == 1)) {
					 first_bin = 0;
					 Result = Result + ( (char) (48  + prev_bin) );
					 sequence_count++;
				  }
				  
				  bin_count=0;
				  prev_bin = current_bin;
				}
				
				point_list_tail = point_list_head;
				point_list_head = point_list_head.next;
				point_list_tail = null;
			 }
			 point_list_tail = null;
			 
			 Result = Result + ( (char) (48  + prev_bin) );
	   	 sequence_count++;

			 if ((point_count < MIN_POINTS) || (sequence_count > MAX_SEQUENCE)) {
				point_count = 0; 
				Result = "";
				return Result;
			 } 
			 
			 point_count = 0;
			 return Result;


	}

	void stroke_record ( int x, int y) {

		int delx, dely;
		double ix, iy;


		if (point_count < MAX_POINTS) {

			s_point new_point = new s_point();

			if ( point_list_head == null ) {
				point_list_head = point_list_tail = new_point;
				min_x = 10000;
				min_y = 10000;
				max_x = -1;
				max_y = -1;
				point_count = 0;
			} else {
			delx = x - point_list_tail.x;
			dely = y - point_list_tail.y;

		  if (java.lang.Math.abs(delx) > java.lang.Math.abs(dely)) {  
       	 iy = point_list_tail.y; 
        
        for (ix = point_list_tail.x; (delx > 0) ? (ix < x) : (ix > x); ix += (delx > 0) ? 1 : -1) {
        
          iy += java.lang.Math.abs(((float) dely / (float) delx)) * (float) ((dely < 0) ?  -1.0 : 1.0);
        
          s_point new_point_i = new s_point();

          point_list_tail.next = new_point_i;
          point_list_tail = new_point_i;
          new_point_i.x = (int) ix;
          new_point_i.y = (int) iy;
//          new_point.next = null;
          
          if (((int) ix) < min_x) min_x = (int) ix;
          if (((int) ix) > max_x) max_x = (int) ix;
          if (((int) iy) < min_y) min_y = (int) iy;
          if (((int) iy) > max_y) max_y = (int) iy;
          point_count++; 

        } 
      } else {  
        ix = point_list_tail.x;
        
        for (iy = point_list_tail.y; (dely > 0) ? (iy < y) : (iy > y); iy += (dely > 0) ? 1 : -1) {
        
          ix += java.lang.Math.abs(((float) delx / (float) dely)) * (float) ((delx < 0) ?  -1.0 : 1.0);
        
          s_point new_point_i = new s_point();

          point_list_tail.next = new_point_i;
          point_list_tail = new_point_i;
          new_point_i.y = (int) iy;
          new_point_i.x = (int) ix;
//          new_point.next = null;
          
          if (((int) ix) < min_x) min_x = (int) ix;
          if (((int) ix) > max_x) max_x = (int) ix;
          if (((int) iy) < min_y) min_y = (int) iy;
          if (((int) iy) > max_y) max_y = (int) iy;
          point_count++; 
          
        } 
      } 
      
      point_list_tail.next = new_point;
      point_list_tail = new_point;
    } 
    
    new_point.x = x;
    new_point.y = y;
//    new_point.next = null; 
 	
	 }
	
	}

	

}

