# exec(open('C:\\projects\\miniPFGA_school\\PCB\\miniFPGA\\assembly_steps.py').read())
# Use this script to show/hide components from 3D view. Helpful for creating PCB assembly documentation.
# run this script from KiCAD command line
# KiKIT front- and backend must be installed
# commands:
#  - load_scenarios(filename)
#    Load all assembly steps from a text file
#    EXAMPLE: 
#    assembly_process = load_screnarios('assembly.txt')
#
#    assembly.txt: every line symbolizes a single step in assmebly. A line may contain one or more reference designators. One refdes. might be used in many steps
#    Example "assembly.txt" below:
#    R1, R2, R3
#    IC1, J4, R4
#    S1
#
#  - show_all()
#    Show all non DNP components.
#
#  - hide_all()
#    Show only the bare PCB
#
#  - show_components(components)
#    show only the components which have reference designators determined by 'components' parameter
#    Example: show_components('R1, R9, IC1')
#
#  - select_components(components)
#    Selects only those components which have reference designators determined by 'components' parameter. This is by default highlighting the component with a distinctive color in 3D quick render
#    Example: select_components('R1, IC1')
#
#  - show_step(step_id, scenarios, highlight=False):
#    Show assembly step defined by <step_id> from a complete assembly process defined by <scenarios>. Highlight current step if optional parameter Highlight=True
#    Parameter step_id:
#    * 0 : Show bare PCB
#    * 1 .. n : Show only step <step_id>
#    * -n .. -1 : Show steps cumulative, including <-step_id>. If highlight=True, the last step will be highlighted.
#    All-in one function for the assembly docmentation. First load the assembly process from text file to a variable. (Example below) 
#    EXAMPLE:
#    assembly_process = load_screnarios('assembly.txt')     # "assembly.txt" same as in the example above
#    show_step(0, assembly_process)                         # Show bare PCB
#    show_step(3, assembly_process)                         # Show only S1 + PCB
#    show_step(-2, assembly_process)                        # Show R1, R2, R3, IC1, J4, R4 + PCB
#    show_step(-3, assembly_process, highlight=True)        # Show R1, R2, R3, IC1, J4, R4, S1 + PCB (S1 highlighted with GREEN in quick render by default settings)
#


import os.path
import re
import io


def show_step(step_id, scenarios=[], highlight=False):
    if step_id:
        if scenarios:
            if len(scenarios) > step_id > 0:
                hide_all()
                show_components(scenarios[step_id-1])
                select_components(scenarios[step_id-1])
            if -len(scenarios) <= step_id < 0:
                hide_all()
                show_components(sum(scenarios[:-step_id], []))
                select_components(scenarios[-1-step_id])
            if not highlight:
                select_components(None)
    else:
        import pcbnew
        hide_all()
        try:
            select_components(pcbnew.GetBoard().Footprints()[0].GetReferenceAsString())
        finally:
            select_components(None)

def show_components(components: list[str]):
    import pcbnew
    board = pcbnew.GetBoard()
    for fp in board.Footprints():
        show_component_3d = fp.GetReferenceAsString().casefold() in components and not fp.IsDNP()
        fp.SetExcludedFromPosFiles(not show_component_3d)


def select_components(components: list[str]):
    import pcbnew
    board = pcbnew.GetBoard()
    for fp in board.Footprints():
        if components and fp.GetReferenceAsString().casefold() in components:
            fp.SetSelected()
        else:
            fp.ClearSelected()
    pcbnew.Refresh()


def show_all():
    import pcbnew
    board = pcbnew.GetBoard()
    for fp in board.Footprints():
        fp.SetExcludedFromPosFiles(False)


def hide_all():
    import pcbnew
    board = pcbnew.GetBoard()
    for fp in board.Footprints():
        fp.SetExcludedFromPosFiles(True)


def load_scenarios(from_data):
    if not from_data:
        return []
    if isinstance(from_data, str):
        if not from_data.strip():
            return []
        if os.path.isfile(from_data):
            # String representing a file
            with open(from_data, 'r') as txt_file:
                return load_scenarios(txt_file)
        else:
            # String representing a single scenario
            return [elem.strip() for elem in re.split(pattern=r' |,|;|\t', string=from_data.casefold()) if elem.strip()] or []
    elif isinstance(from_data, io.TextIOBase) or (hasattr(from_data, 'readline') and callable(from_data.readline)):
        # File like object
        scenarios = []
        while line := from_data.readline():
            if not os.path.isfile(line):
                scene = load_scenarios(line) or []
                if scene:
                    scenarios.append(scene)
        return scenarios[0] if len(scenarios) == 1 else scenarios

    # list of components
    return [elem.strip().casefold() for elem in from_data]
