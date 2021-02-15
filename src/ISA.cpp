#include"ISA.h"
#include"error_handler.h"
#include<stdio.h>
#include<cstring>

bool check_conditional(CPU & cpu, byte conditional)
{
  cpu.cycles++;
  switch(conditional)
  {
    case COND_AL:
      return true;
    case COND_EQ:
      return (cpu.Z == 1);
    case COND_NE:
      return (cpu.Z == 0);
    case COND_CS:
      return (cpu.C == 1);
    case COND_CC:
      return (cpu.C == 0);
    case COND_MI:
      return (cpu.N == 1);
    case COND_PL:
      return (cpu.N == 0);
    case COND_OS:
      return (cpu.O == 1);
    case COND_OC:
      return (cpu.O == 0);
    case COND_HI:
      return ((cpu.C == 1) && (cpu.Z == 0));
    case COND_LS:
      return ((cpu.C == 0) && (cpu.Z == 1));
    case COND_GE:
      return ( ((cpu.N == 1) && (cpu.O == 1)) || ((cpu.N == 0) && (cpu.O == 0)) );
    case COND_LT:
      return ( ((cpu.N == 1) && (cpu.O == 0)) || ((cpu.N == 0) && (cpu.O == 1)) );
    case COND_GT:
      return ( (cpu.Z == 0) && ( ((cpu.N == 1) && (cpu.O == 1)) || ((cpu.N == 0) && (cpu.O == 0)) ) );
    case COND_LE:
      return ( (cpu.Z == 1) && ( ((cpu.N == 1) && (cpu.O == 0)) || ((cpu.N == 0) && (cpu.O == 1)) ) );
    case COND_NV:
      return false;
    default:
      return false;
  }
  return false;
}

void op_end(struct CPU & cpu, instruction & ins)
{
  cpu.cycles++;
  printf("End opcode recieved.\n");
}

void op_add(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    //Add instruction code
    word destination, operand1, operand2, shift;
    uint64_t result;
    destination = ins.Rd;
    operand1 = cpu.R[ins.Rn];

    //Immediate value
    if(ins.I)
      operand2 = ins.immed;
    else
      operand2 = (cpu.R[ins.op2] + (ins.shift)) & MAX_U;

    LOG("ADD\nop1 : " << operand1);
    LOG("op2 : " << operand2);
    LOG("Result reg: " << destination);


    cpu.cycles++;
    result = (dword) operand1 + (dword) operand2;

    /* Setting the flags... */
    if(ins.S)
    {
      LOG("S set");
      cpu.Z = 0;
      cpu.N = 0;
      cpu.C = 0;
      cpu.O = 0;
      cpu.cycles += 4;
      if((result & MAX_U) == 0)
        cpu.Z = 1;
      if((result & MAX_U) & SIGN_BIT)
        cpu.N = 1;
      //Carry over...
      if(result & (MAX_U + 1))
        cpu.C = 1;

      //Now for overflow checks
      if( !( (operand1 & SIGN_BIT) ^ (operand2 & SIGN_BIT) ) )
    	 if( (result ^ operand1) & SIGN_BIT)
          cpu.O = 1;

      LOG("Flags:\nZ: " << (bool) cpu.Z << "\nN: " << (bool) cpu.N << "\nC: " << (bool) cpu.C << "\nO: " << (bool) cpu.O);
    }

    cpu.R[destination] = result & MAX_U;
    LOG("RESULT: " << (word) cpu.R[destination]);
  }
}

void op_sub(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    //Sub instruction code
    word destination, operand1, operand2, shift;
    uint64_t result;
    destination = ins.Rd;
    operand1 = cpu.R[ins.Rn];

    //Immediate value
    if(ins.I)
      operand2 = ins.immed;
    else
      operand2 = (cpu.R[ins.op2] + (ins.shift)) & MAX_U;

    LOG("SUB\nop1 : " << operand1);
    LOG("op2 : " << operand2);
    LOG("Result reg: " << destination);


    cpu.cycles++;
    result = (dword) operand1 - (dword) operand2;

    /* Setting the flags... */
    if(ins.S)
    {
      LOG("S set");
      cpu.Z = 0;
      cpu.N = 0;
      cpu.C = 0;
      cpu.O = 0;
      cpu.cycles += 4;
      if((result & MAX_U) == 0)
        cpu.Z = 1;
      if((result & MAX_U) & SIGN_BIT)
        cpu.N = 1;
      //Carry over... inverted carry flag
      if(((int32_t) (result & MAX_U)) >= 0)
        cpu.C = 1;

      //Now for overflow checks
      if( ( (operand1 & SIGN_BIT) ^ (operand2 & SIGN_BIT) ) )
       if( (result ^ operand1) & SIGN_BIT)
          cpu.O = 1;

      LOG("Flags:\nZ: " << (bool) cpu.Z << "\nN: " << (bool) cpu.N << "\nC: " << (bool) cpu.C << "\nO: " << (bool) cpu.O);
    }

    cpu.R[destination] = result & MAX_U;
    LOG("RESULT: " << (word) cpu.R[destination]);
  }
}

void op_ldr(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    word destination, operand;
    destination = ins.Rn;

    if(ins.I)
      operand = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      operand = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    /* If the address is outside of memory bound, sigsegv */
    if(operand > (cpu.mem->size - 4))
      error_handler(ERR_SEG, &cpu);

    memcpy(cpu.R + destination, cpu.mem->data + operand, 4);
    LOG("LDR\nMoved " << cpu.R[destination] << " from address " << operand << " into register " << destination);
  }
}

void op_ldrh(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    word destination, operand;
    destination = ins.Rn;

    if(ins.I)
      operand = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      operand = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    /* If the address is outside of memory bound, sigsegv */
    if(operand > (cpu.mem->size - 2))
      error_handler(ERR_SEG, &cpu);

    memcpy(cpu.R + destination, cpu.mem->data + operand, 2);
    LOG("LDRH\nMoved " << cpu.R[destination] << " from address " << operand << " into register " << destination);
  }
}

void op_ldrb(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    word destination, operand;
    destination = ins.Rn;

    if(ins.I)
      operand = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      operand = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    /* If the address is outside of memory bound, sigsegv */
    if(operand > (cpu.mem->size - 1))
      error_handler(ERR_SEG, &cpu);

    memcpy(cpu.R + destination, cpu.mem->data + operand, 1);
    LOG("LDRB\nMoved " << cpu.R[destination] << " from address " << operand << " into register " << destination);
  }
}

void op_str(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    word operand, destination;
    operand = ins.Rn;

    if(ins.I)
      destination = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      destination = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    /* If the address is outside of memory bound, sigsegv */
    if(destination > (cpu.mem->size - 4))
      error_handler(ERR_SEG, &cpu);

    memcpy(cpu.mem->data + destination, cpu.R + operand, 4);
    LOG("STR\nMoved " << cpu.R[operand] << " from register " << (word) ins.Rn << " into address " << destination);
  }
}

void op_strh(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    word operand, destination;
    operand = ins.Rn;

    if(ins.I)
      destination = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      destination = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    /* If the address is outside of memory bound, sigsegv */
    if(destination > (cpu.mem->size - 2))
      error_handler(ERR_SEG, &cpu);

    memcpy(cpu.mem->data + destination, cpu.R + operand, 2);
    LOG("STRH\nMoved " << cpu.R[operand] << " from register " << (word) ins.Rn << " into address " << destination);
  }
}

void op_strb(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    word operand, destination;
    operand = ins.Rn;

    if(ins.I)
      destination = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      destination = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    /* If the address is outside of memory bound, sigsegv */
    if(destination > (cpu.mem->size - 1))
      error_handler(ERR_SEG, &cpu);

    memcpy(cpu.mem->data + destination, cpu.R + operand, 1);
    LOG("STRB\nMoved " << cpu.R[operand] << " from register " << (word) ins.Rn << " into address " << destination);
  }
}

void op_mov(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    word destination, operand, shift;
    destination = ins.Rn;

    //TODO - implement checks for overflow

    //op2 becomes shift without I set
    if(ins.I)
      operand = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      operand = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    cpu.R[destination] = operand;

    LOG("MOV\nMoved " << operand << " into register "  << destination);
  }
}

void op_prnr(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    /* Print Register */
    /* Flags stay the same */
    /* I and S become number of registers */
    byte num_reg = ((ins.I << 1) | (ins.S)) + 1;
    LOG("Number of registers to print: " << (int) num_reg);
    cpu.cycles += num_reg;

    switch(ins.shift & 0b000011)
    {
      case 0:
      {
        if(num_reg >= 1)
          printf("R%u: %u\n", ins.Rn, cpu.R[ins.Rn]);
        if(num_reg >= 2)
          printf("R%u: %u\n", ins.Rd, cpu.R[ins.Rd]);
        if(num_reg >= 3)
          printf("R%u: %u\n", ins.op2, cpu.R[ins.op2]);
        if(num_reg == 4)
          printf("R%u: %u\n", ins.shift >> 2, cpu.R[ins.shift >> 2]);
        break;
      }
      case 1:
      {
        if(num_reg >= 1)
          printf("R%u: %d\n", ins.Rn, cpu.R[ins.Rn]);
        if(num_reg >= 2)
          printf("R%u: %d\n", ins.Rd, cpu.R[ins.Rd]);
        if(num_reg >= 3)
          printf("R%u: %d\n", ins.op2, cpu.R[ins.op2]);
        if(num_reg == 4)
          printf("R%u: %d\n", ins.shift >> 2, cpu.R[ins.shift >> 2]);
        break;
      }
      case 2:
      {
        if(num_reg >= 1)
          printf("R%u: 0x%X\n", ins.Rn, cpu.R[ins.Rn]);
        if(num_reg >= 2)
          printf("R%u: 0x%X\n", ins.Rd, cpu.R[ins.Rd]);
        if(num_reg >= 3)
          printf("R%u: 0x%X\n", ins.op2, cpu.R[ins.op2]);
        if(num_reg == 4)
          printf("R%u: 0x%X\n", ins.shift >> 2, cpu.R[ins.shift >> 2]);
        break;
      }
      case 3:
      {
        if(num_reg >= 1)
          printf("R%u: %c\n", ins.Rn, (char) cpu.R[ins.Rn]);
        if(num_reg >= 2)
          printf("R%u: %c\n", ins.Rd, (char) cpu.R[ins.Rd]);
        if(num_reg >= 3)
          printf("R%u: %c\n", ins.op2, (char) cpu.R[ins.op2]);
        if(num_reg == 4)
          printf("R%u: %c\n", ins.shift >> 2, (char) cpu.R[ins.shift >> 2]);
        break;
      }
    }
  }
}

ISA::ISA()
{
  /*Initialize the handlers for the opcodes */
  for(int i = 0; i < OP_END; i++)
    handlers[i] = NULL;

  handlers[OP_ADD] = op_add;
  handlers[OP_SUB] = op_sub;
  handlers[OP_LDR] = op_ldr;
  handlers[OP_LDRH] = op_ldrh;
  handlers[OP_LDRB] = op_ldrb;
  handlers[OP_STR] = op_str;
  handlers[OP_STRH] = op_strh;
  handlers[OP_STRB] = op_strb;
  handlers[OP_MOV] = op_mov;
  handlers[OP_CMP] = NULL;
  handlers[OP_PRNR] = op_prnr;
  handlers[OP_PRNM] = NULL;
  handlers[OP_END] = op_end;

}
