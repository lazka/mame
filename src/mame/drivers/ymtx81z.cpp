// license:BSD-3-Clause
// copyright-holders:AJR
/****************************************************************************

    Skeleton driver for Yamaha TX81Z.

****************************************************************************/

#include "emu.h"
//#include "bus/midi/midi.h"
#include "cpu/m6800/m6801.h"
#include "machine/clock.h"
#include "machine/nvram.h"
#include "sound/ym2151.h"
#include "video/hd44780.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"

class ymtx81z_state : public driver_device
{
public:
	ymtx81z_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{
	}

	void tx81z(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	HD44780_PIXEL_UPDATE(lcd_pixel_update);
	void palette_init(palette_device &palette);

	void mem_map(address_map &map);

	required_device<hd6303x_cpu_device> m_maincpu;
};

HD44780_PIXEL_UPDATE(ymtx81z_state::lcd_pixel_update)
{
	if (x < 5 && y < 8 && line < 2 && pos < 16)
		bitmap.pix16(line * 8 + y, pos * 6 + x) = state;
}

void ymtx81z_state::palette_init(palette_device &palette)
{
	palette.set_pen_color(0, rgb_t(131, 136, 139));
	palette.set_pen_color(1, rgb_t( 92,  83,  88));
}

void ymtx81z_state::machine_start()
{
	membank("rombank")->configure_entries(0, 2, memregion("program")->base(), 0x8000);
}

void ymtx81z_state::mem_map(address_map &map)
{
	map(0x0000, 0x001f).m(m_maincpu, FUNC(hd6303x_cpu_device::hd6301x_io));
	map(0x0040, 0x00ff).ram(); // internal RAM
	map(0x2000, 0x2001).rw("ymsnd", FUNC(ym2414_device::read), FUNC(ym2414_device::write));
	map(0x4000, 0x4001).rw("lcdc", FUNC(hd44780_device::read), FUNC(hd44780_device::write));
	map(0x6000, 0x7fff).ram().share("nvram");
	map(0x8000, 0xffff).bankr("rombank");
}

static INPUT_PORTS_START(tx81z)
INPUT_PORTS_END

void ymtx81z_state::tx81z(machine_config &config)
{
	HD6303X(config, m_maincpu, 7.15909_MHz_XTAL); // HD63B03XP
	m_maincpu->set_addrmap(AS_PROGRAM, &ymtx81z_state::mem_map);
	m_maincpu->out_p6_cb().set_membank("rombank").bit(3);

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0); // TC5564PL-15 + CR2032 battery

	CLOCK(config, "midiclock", 500_kHz_XTAL);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_LCD));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	screen.set_screen_update("lcdc", FUNC(hd44780_device::screen_update));
	screen.set_size(6*16, 8*2);
	screen.set_visarea_full();
	screen.set_palette("palette");

	PALETTE(config, "palette", FUNC(ymtx81z_state::palette_init), 2);

	hd44780_device &lcdc(HD44780(config, "lcdc", 0));
	lcdc.set_lcd_size(2, 16);
	lcdc.set_pixel_update_cb(FUNC(ymtx81z_state::lcd_pixel_update));

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	ym2414_device &ymsnd(YM2414(config, "ymsnd", 7.15909_MHz_XTAL / 2));
	ymsnd.irq_handler().set_inputline(m_maincpu, HD6301_IRQ_LINE);
	ymsnd.add_route(0, "lspeaker", 0.60);
	ymsnd.add_route(1, "rspeaker", 0.60);
}

ROM_START(tx81z)
	ROM_REGION(0x10000, "program", 0)
	ROM_SYSTEM_BIOS(0, "last", "Last Revision")
	ROMX_LOAD("tx81z-last-revision.ic15", 0x00000, 0x10000, CRC(2d635c40) SHA1(cae098724079d7b8a711351c3132d7332ca43751), ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "v16", "Version 1.6")
	ROMX_LOAD("tx81z-v1.6.ic15", 0x00000, 0x10000, CRC(ab9b7347) SHA1(208a72c0dc615825c442240e520a6a3c5fe860ea), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "v15", "Version 1.5")
	ROMX_LOAD("tx81z-27512-image-version-1_5.ic15", 0x00000, 0x10000, CRC(64ab615b) SHA1(82cdd8637caf3828aee5ccf25f1ed92ae5d65d3b), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(3, "v14", "Version 1.4")
	ROMX_LOAD("tx81z-v1.4.ic15", 0x00000, 0x10000, CRC(694a13e2) SHA1(0b656a8040748f1e4ee73df2a9436fee1c724be8), ROM_BIOS(3))
	ROM_SYSTEM_BIOS(4, "v13", "Version 1.3")
	ROMX_LOAD("tx81z-v1.3.ic15", 0x00000, 0x10000, CRC(7abd5a61) SHA1(93ae5498ce650fe09952ca81c9ac3821f44c20dc), ROM_BIOS(4))
	ROM_SYSTEM_BIOS(5, "v12", "Version 1.2")
	ROMX_LOAD("tx81z-v1.2.ic15", 0x00000, 0x10000, CRC(8378f744) SHA1(d76b573d4deb67f0e1553c9c17804e970b392803), ROM_BIOS(5))
	ROM_SYSTEM_BIOS(6, "v11", "Version 1.1")
	ROMX_LOAD("tx81z-v1.1.ic15", 0x00000, 0x10000, CRC(3e78db9f) SHA1(52eafb9a1cb3ffb68e8b8dd7a2b85d9e607f9e1c), ROM_BIOS(6))
	ROM_SYSTEM_BIOS(7, "v10", "Version 1.0")
	ROMX_LOAD("tx81z-27512-image-first-version-1_0.ic15", 0x00000, 0x10000, CRC(2f9628fa) SHA1(ce62dfb9a86da092c469fd25328b5447375f5bb2), ROM_BIOS(7))
ROM_END

SYST(1987, tx81z, 0, 0, tx81z, tx81z, ymtx81z_state, empty_init, "Yamaha", "TX81Z FM Tone Generator", MACHINE_IS_SKELETON)
//SYST(1988, dx11, 0, 0, dx11, dx11, ymtx81z_state, empty_init, "Yamaha", "DX11 Digital Programmable Algorithm Synthesizer", MACHINE_IS_SKELETON)
