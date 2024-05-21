#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/* Some FB colors*/
#define FB_GREEN 2
#define FB_DARK_GREY 8

/** outb:
 *  Sends the given data to the given I/O port. Defined in io.s
 *
 *  @param port The I/O port to send the data to
 *  @param data The data to send to the I/O port
 */
extern void outb(unsigned short port, unsigned char data);

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos);

/** write_chars
 *  Write the array of chars to the framebuffer at the given postion, and moves the cursor to the end of the chars.
 *
 * @param chars The array of character
 * @param pos   The location in the framebuffer
*/
void write_chars(char chars[], unsigned int pos);

#endif /* INCLUDE_IO_H */