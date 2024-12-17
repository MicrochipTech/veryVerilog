from io import StringIO

from ddt import ddt, data
from unittest import TestCase, main
from assembly_steps import load_scenarios


@ddt
class Test(TestCase):
    @data('', [], {}, tuple(), None, '   ', ';', ',', ' ', '    ', '  ; , ;;,,  ;', ';\n', '\t', '\r', '\n\r', '\t ;\r')
    def test_load_scenarios_empty_input(self, value):
        self.assertEqual([], load_scenarios(value))

    @data('R1', 'r1', 'r11', 'U97  ', '   U3', '  u42 ')
    def test_load_scenarios_single_ref_str(self, value:str):
        self.assertEqual([value.strip().casefold()], load_scenarios(value), )

    @data('µ1', 'ű1', 'ϕ11', '☎97  ', '  Ű4\N{NO-BREAK SPACE}2 ')
    def test_load_scenarios_unicode_ref_str(self, value:str):
        self.assertEqual([value.strip().casefold()], load_scenarios(value), )

    @data(['R1'], ['r1'], ['r11'], ['U97  '], ['   U3'], ['  u42 '])
    def test_load_scenarios_single_ref_list(self, value:str):
        self.assertEqual([value[0].strip().casefold()], load_scenarios(value), )

    @data('R1 R2 R3', 'r1 R2 R3', 'r1   r2 r3  ', ' r1  r2  r3  ', 'r1  r2 r3  ', 'R1, r2, r3', 'r1;r2;r3', 'R1 R2;r3', 'R1\tR2;r3', 'R1\t\t R2;r3')
    def test_load_scenarios_multi_ref_str(self, value:str):
        self.assertEqual(['r1', 'r2', 'r3'], load_scenarios(value), )

    @data('R1 R2 R3\nU1 u2 u3', 'r1 R2 R3\n\nU1 u2 u3', 'r1 R2 R3\n  \nU1 u2 u3')
    def test_load_scenarios_multi_line_stream(self, value:str):
        with StringIO(value) as str_stream:
            self.assertEqual([['r1', 'r2', 'r3'], ['u1', 'u2', 'u3']], load_scenarios(str_stream), )

    @data('R1 R2 R3', '\nR1 R2 R3', 'R1 R2 R3\n', 'r1 R2 R3\n\n', '\nr1 R2 R3\n')
    def test_load_scenarios_single_line_stream(self, value:str):
        with StringIO(value) as str_stream:
            self.assertEqual(['r1', 'r2', 'r3'], load_scenarios(str_stream), )

    @data('', ' \n', ' \n\r', ' \r\n', '  ', '; ; ;', ' ;\n \n;  ')
    def test_load_scenarios_empty_stream(self, value:str):
        with StringIO(value) as str_stream:
            self.assertEqual([], load_scenarios(str_stream), )


if __name__ == '__main__':
    main(verbosity=2)
