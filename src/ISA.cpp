#include"ISA.h"
#include"error_handler.h"
#include"init.h"
#include<stdio.h>
#include<cstring>

bool check_conditional(CPU & cpu, byte conditional)
{
  cpu.cycles++;
  //Checks the conditionals and returns true/false if the condition is met.
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
  printf("\nEnd opcode recieved.\n");
}

void op_add(struct CPU & cpu, instruction & ins)
{
  LOG("ADD");
  if(check_conditional(cpu, ins.conditional))
  {
    //Add instruction code
    word destination, operand1, operand2;
    uint64_t result;
    destination = ins.Rd;
    operand1 = cpu.R[ins.Rn];

    //Immediate value
    if(ins.I)
      operand2 = ins.immed;
    else
      operand2 = (cpu.R[ins.op2] + (ins.shift)) & MAX_U;

    LOG("op1 : " << operand1);
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
    LOG("SUB");
    //Sub instruction code
    word destination, operand1, operand2;
    uint64_t result;
    destination = ins.Rd;
    operand1 = cpu.R[ins.Rn];

    //Immediate value
    if(ins.I)
      operand2 = ins.immed;
    else
      operand2 = (cpu.R[ins.op2] + (ins.shift)) & MAX_U;

    LOG("op1 : " << operand1);
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
    LOG("LDR");
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
    LOG("Moved " << cpu.R[destination] << " from address " << operand << " into register " << destination);
  }
}

void op_ldrh(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("LDRH");
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
    LOG("Moved " << cpu.R[destination] << " from address " << operand << " into register " << destination);
  }
}

void op_ldrb(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("LDRB");
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
    LOG("Moved " << cpu.R[destination] << " from address " << operand << " into register " << destination);
  }
}

void op_str(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("STR");
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
    LOG("Moved " << cpu.R[operand] << " from register " << (word) ins.Rn << " into address " << destination);
  }
}

void op_strh(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("STRH");
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
    LOG("Moved " << cpu.R[operand] << " from register " << (word) ins.Rn << " into address " << destination);
  }
}

void op_strb(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("STRB");
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
    LOG("Moved " << cpu.R[operand] << " from register " << (word) ins.Rn << " into address " << destination);
  }
}

void op_mov(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("MOV");
    word destination, operand;
    destination = ins.Rn;

    //TODO - implement checks for overflow

    //op2 becomes shift without I set
    if(ins.I)
      operand = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      operand = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    cpu.R[destination] = operand;

    LOG("Moved " << operand << " into register "  << destination);
  }
}

void op_cmp(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("CMP");
    //cmp instruction code is similar to sub, but result is never stored.
    word operand1, operand2;
    operand1 = cpu.R[ins.Rn];
    uint64_t result;

    //Immediate value
    if(ins.I)
      operand2 = ((ins.Rd << 10) | (ins.immed)) % MAX_U;
    else
      operand2 = ((cpu.R[ins.Rd]) + ins.immed) % MAX_U;

    LOG("op1 : " << operand1);
    LOG("op2 : " << operand2);

    cpu.cycles++;
    result = (dword) operand1 - (dword) operand2;

    /* Setting the flags... */

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
}

void op_prnr(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("PRNR");
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

void op_prnm(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("PRNM");
    word address;
    /* Print Memory */
    /* Flags stay the same */
    /* I and S become number of registers */
    byte num_bytes = (ins.Rn >> 2);
    if(num_bytes == 0)
      num_bytes = 1;
    else if(num_bytes == 1)
      num_bytes = 2;
    else if(num_bytes == 2)
      num_bytes = 4;
    else if(num_bytes == 3)
      num_bytes = 8;

    byte specifier = (ins.Rn & 0b0011);
    LOG("bytes " << (int) num_bytes);
    LOG("Specifier " << (int) specifier);
    LOG("I : " << (int) ins.I);
    LOG("S : " << (int) ins.S);

    cpu.cycles += 2;

    /* Now we need to find out the address that we want to print */
    //
    if(ins.I)
      address = ((ins.Rd << 10) | (ins.immed));
    else
      address = ((cpu.R[ins.Rd] + (ins.immed)) & MAX_U);

    //print_all_memory(*(cpu.mem));

    LOG("MEMSIZE: " << cpu.mem->size);
    if(address > (cpu.mem->size - num_bytes))
      error_handler(ERR_SEG, &cpu);

    dword result = 0;
    memcpy(&result, cpu.mem->data + address, num_bytes);

    switch(specifier)
    {
      case 0:
        printf("Address %" PRIu32 ": %" PRIu64 "\n", address, result);
        break;
      case 1:
        printf("Address %" PRIu32 ": %" PRIu64 "\n", address, result);
        break;
      case 2:
        printf("Address %" PRIu32 ": 0x%" PRIX64 "\n", address, result);
        break;
      case 3:
        printf("Address %" PRIu32 ": %c\n", address, (char) result);
        break;
    }
  }
}

void op_brn(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("BRN");
    //I becomes the negative or positive flag
    //If I is set, we need to subtract from the PC (R14)
    word offset = ((ins.S << 18) | (ins.Rn <<  14) | (ins.Rd << 10) | ins.immed) << 2;
    cpu.cycles += 2;
    if(ins.I)
    {
      LOG("Branched -" << (unsigned int) offset << " from PC value: " << (unsigned int) cpu.R[14]);
      cpu.R[14] = cpu.R[14] - offset;
    }
    else
    {
      LOG("Branched +" << (unsigned int) offset << " from PC value: " << (unsigned int) cpu.R[14]);
      cpu.R[14] = cpu.R[14] + offset;
    }
    LOG("New PC value: " << (unsigned int) cpu.R[14]);
  }
}

void op_brnl(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    //Branch with LR save, works the same but the current PC
    //Value [R14] is saved to LR [R13]
    LOG("BRNL");
    //I becomes the negative or positive flag
    //If I is set, we need to subtract from the PC (R14)
    word offset = ((ins.S << 18) | (ins.Rn <<  14) | (ins.Rd << 10) | ins.immed) << 2;
    cpu.cycles += 3;
    cpu.R[13] = cpu.R[14];
    if(ins.I)
    {
      LOG("Branched -" << (unsigned int) offset << " from PC value: " << (unsigned int) cpu.R[14]);
      cpu.R[14] = cpu.R[14] - offset;
    }
    else
    {
      LOG("Branched +" << (unsigned int) offset << " from PC value: " << (unsigned int) cpu.R[14]);
      cpu.R[14] = cpu.R[14] + offset;
    }
    LOG("New PC value: " << (unsigned int) cpu.R[14]);
  }
}

void op_lsl(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("LSL");
    cpu.cycles++;
    //We will perform a logical shift left...
    //Rd = destination
    //Rn - operand 1
    //Op2 = amount to left shift by
    word destination, operand1, operand2;
    dword result;

    destination = ins.Rd;
    operand1 = cpu.R[ins.Rn];

    //I is set, so operand 2 becomes an immediate value
    if(ins.I)
      operand2 = ins.immed;
    else
      operand2 = (cpu.R[ins.op2] + ins.shift) & MAX_U;

    //Peform the shift!
    result = operand1 << operand2;

    LOG("op1 : " << operand1);
    LOG("op2 : " << operand2);
    LOG("Result reg: " << destination);

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
    }
    cpu.R[destination] = (result & MAX_U);
    LOG("RESULT: " << cpu.R[destination]);
  }
}

void op_lsr(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("LSR");
    cpu.cycles++;
    //We will perform a logical shift left...
    //Rd = destination
    //Rn - operand 1
    //Op2 = amount to right shift by
    word destination, operand1, operand2;
    dword result;

    destination = ins.Rd;
    operand1 = cpu.R[ins.Rn];

    //I is set, so operand 2 becomes an immediate value
    if(ins.I)
      operand2 = ins.immed;
    else
      operand2 = (cpu.R[ins.op2] + ins.shift) & MAX_U;

    //Peform the shift!
    result = operand1 >> operand2;

    LOG("op1 : " << operand1);
    LOG("op2 : " << operand2);
    LOG("Result reg: " << destination);

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
    }
    cpu.R[destination] = (result & MAX_U);
    LOG("RESULT: " << cpu.R[destination]);
  }
}

void op_asr(struct CPU & cpu, instruction & ins)
{
  if(check_conditional(cpu, ins.conditional))
  {
    LOG("ASR");
    //We will do sign extension here...
    cpu.cycles++;
    //We will perform a logical shift left...
    //Rd = destination
    //Rn - operand 1
    //Op2 = amount to right shift by
    word destination, operand1, operand2;
    dword result;

    destination = ins.Rd;
    operand1 = cpu.R[ins.Rn];

    //I is set, so operand 2 becomes an immediate value
    if(ins.I)
      operand2 = ins.immed;
    else
      operand2 = (cpu.R[ins.op2] + ins.shift) & MAX_U;

    //Peform the shift!
    result = operand1 >> operand2;

    LOG("op1 : " << operand1);
    LOG("op2 : " << operand2);
    LOG("Result reg: " << destination);

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
    }
    cpu.R[destination] = (result & MAX_U);
    LOG("RESULT: " << cpu.R[destination]);
  }
}


ISA::ISA()
{
  /*Initialize the handlers for the opcodes */
  for(int i = 0; i < OP_END + 1; i++)
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
  handlers[OP_CMP] =  op_cmp;
  handlers[OP_PRNR] = op_prnr;
  handlers[OP_PRNM] = op_prnm;
  handlers[OP_BRN] = op_brn;
  handlers[OP_BRNL] = op_brnl;
  handlers[OP_LSL] = op_lsl;
  handlers[OP_LSR] = op_lsr;
  handlers[OP_ASR] = op_asr;
  handlers[OP_END] = op_end;

}
