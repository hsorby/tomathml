
#pragma once


const char * expected_test_result_1 =
R"JK(<?xml version="1.0" encoding="UTF-8"?>
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <apply>
    <eq />
    <ci>
      a
    </ci>
    <ci>
      b
    </ci>
  </apply>
</math>
)JK";

const char * expected_test_result_2 =
R"JK(<?xml version="1.0" encoding="UTF-8"?>
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <apply>
    <eq />
    <ci>
      a
    </ci>
    <ci>
      b
    </ci>
  </apply>
  <apply>
    <eq />
    <ci>
      c
    </ci>
    <ci>
      d
    </ci>
  </apply>
</math>
)JK";

const char * expected_test_result_3 =
R"JK(<?xml version="1.0" encoding="UTF-8"?>
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <apply>
    <eq />
    <apply>
      <diff />
      <bvar>
        <ci>
          t
        </ci>
      </bvar>
      <ci>
        x
      </ci>
    </apply>
    <cn cellml:units="dimensionless" xmlns:cellml="http://www.cellml.org/cellml/2.0#">
      3
    </cn>
  </apply>
</math>
)JK";

const char * expected_test_result_4 =
R"JK(<?xml version="1.0" encoding="UTF-8"?>
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <apply>
    <eq />
    <apply>
      <diff />
      <bvar>
        <ci>
          t
        </ci>
      </bvar>
      <ci>
        y
      </ci>
    </apply>
    <apply>
      <minus />
      <apply>
        <times />
        <ci>
          mu
        </ci>
        <apply>
          <minus />
          <cn cellml:units="dimensionless" xmlns:cellml="http://www.cellml.org/cellml/2.0#">
            1
          </cn>
          <apply>
            <power />
            <ci>
              x
            </ci>
            <cn cellml:units="dimensionless" xmlns:cellml="http://www.cellml.org/cellml/2.0#">
              2
            </cn>
          </apply>
        </apply>
        <ci>
          y
        </ci>
      </apply>
      <ci>
        x
      </ci>
    </apply>
  </apply>
</math>
)JK";

const char * expected_test_result_5 =
R"JK(<?xml version="1.0" encoding="UTF-8"?>
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <apply>
    <eq />
    <ci>
      a
    </ci>
    <ci>
      b
    </ci>
  </apply>
  <apply>
    <eq />
    <ci>
      c
    </ci>
    <apply>
      <plus />
      <ci>
        d
      </ci>
      <exponentiale />
    </apply>
  </apply>
</math>
)JK";

const char * expected_test_result_6 =
R"JK(<?xml version="1.0" encoding="UTF-8"?>
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <apply>
    <eq />
    <ci>
      a
    </ci>
    <apply>
      <minus />
      <ci>
        b
      </ci>
      <cn cellml:units="kilogram" xmlns:cellml="http://www.cellml.org/cellml/2.0#">
        5
      </cn>
    </apply>
  </apply>
</math>
)JK";

const char * expected_test_result_7 =
R"JK(<?xml version="1.0" encoding="UTF-8"?>
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <apply>
    <eq />
    <ci>
      a
    </ci>
    <apply>
      <plus />
      <ci>
        b
      </ci>
      <cn>
        3
      </cn>
    </apply>
  </apply>
</math>
)JK";
