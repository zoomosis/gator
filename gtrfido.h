#ifndef __GTRFIDO_H__
#define __GTRFIDO_H__

char *show_address(unsigned short zone, unsigned short net,
		   unsigned short node, unsigned short point, char *domain);

char *show_4d(unsigned short zone, unsigned short net,
	      unsigned short node, unsigned short point);

char *show_3d(unsigned short zone, unsigned short net, unsigned short node);

char *show_2d(unsigned short net, unsigned short node);

char *show_product(unsigned short prod_code);

#endif
