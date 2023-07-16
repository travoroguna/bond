//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#ifndef BOND_SPAN_H
#define BOND_SPAN_H

namespace bond {
struct Span {
  uint32_t module_id;
  uint32_t start{};
  uint32_t end{};
  uint32_t line{};

  Span(uint32_t module_id, uint32_t start, uint32_t end, uint32_t line);
};
}

#endif //BOND_SPAN_H
