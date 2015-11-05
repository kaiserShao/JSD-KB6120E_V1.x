void  _task_Sample_TSP( void )
{
	const enum enumSamplerSelect	SamplerSelect = Q_TSP;
	const enum enumPumpSelect       PumpSelect = PP_TSP;

	BOOL	PumpState;

	uint32_t	tt, now_minute = get_Now() / 60u;

	struct	uFile_TSP	File;
	
	uint16_t	fname;	//	��ǰ��Ч�ļ�
	uint32_t	start;	//	������ʼʱ��
	uint8_t 	iloop;	//	��ǰ��������
	
	PumpState = FALSE;

	Q_Pump[PumpSelect].xp_state = FALSE;

	
 	start = SampleSet[SamplerSelect].start;
 	iloop = SampleSet[SamplerSelect].iloop;
	
	if ( 0u != start )
	{
		if ( 0u == iloop )
		{	//	��ʼ����������
			switch ( Configure.Mothed_Delay )
			{
			case enumByDelay:
				start = start + SampleSet[SamplerSelect].delay1;
				break;
			default:
			case enumByAccurate:
				{
					uint16_t	start_hour_minute = start % 1440u;
					
					if ( start_hour_minute > SampleSet[SamplerSelect].delay1 )
					{
						start += 1440u;	//	�Ƴٵ���������ʱ�����
					}
					start = start - start_hour_minute + SampleSet[SamplerSelect].delay1;
				}
				break;
			}
			iloop = 1u;
		}
		
		while (( ! SampleSwitch[SamplerSelect].Clean ) && ( iloop <= SampleSet[SamplerSelect].set_loops ))
		{

			SampleSet[SamplerSelect].start = start;
			SampleSet[SamplerSelect].iloop = iloop;
			SampleSetSave();
			
			Q_Sampler[SamplerSelect].loops = iloop;

			//	0.��ʱ���ȴ���������ʱ���߼��״̬��
			while (( ! SampleSwitch[SamplerSelect].Clean ) && ( now_minute < start ))
			{
				Q_Sampler[SamplerSelect].timer = (uint16_t)( start - now_minute );
				

				if ( SampleSwitch[SamplerSelect].Pause )
				{
					PumpState = FALSE;
					Q_Sampler[SamplerSelect].state = state_PAUSE;
				}
				else
				{
					PumpState = FALSE;
					Q_Sampler[SamplerSelect].state = state_SUSPEND;
				}

				if ( PumpState != Q_Pump[PumpSelect].xp_state )
				{
					Pump_OutCmd( PumpSelect, PumpState );
					Q_Pump[PumpSelect].xp_state = PumpState;
				}




				delay( 1000u );

				now_minute = get_Now() / 60u;

				//	��ʾ��ѯ����
				Q_Pump[PumpSelect].sum_time = 0u;
				Q_Pump[PumpSelect].vd       = 0.0f;
				Q_Pump[PumpSelect].vnd      = 0.0f;

			}

			//	1.0 ���������Ҫ��ʼ��������������Ч����
			fname = SampleSet[SamplerSelect].FileNum;
			if ( ++fname > FileNum_Max ){  fname = 1u; }
			
			File_Load_TSP( fname, &File );
			if ( 0u == File.sample_begin )
			{
				File.sum_min = 0u;
				File.max_pr  =
				File.sum_tr  =
				File.sum_pr  =
				File.vd      =
				File.vnd     = 0.0f;
				File.sum_Ba  = 0.0f;
        
				

			File_Save_TSP( fname, &File );
			}

			//	1.������ִ�в���
			while ( ! SampleSwitch[SamplerSelect].Clean )	//	NOT ����ʱ�䵽?
			{

				switch ( Configure.Mothed_Sample )
				{
				default:
				case enumBySet:	//	���ݲ���ʱ���������У��۳����磬������ʱ������
					if ( now_minute < ( start + SampleSet[SamplerSelect].sample_1 ))
					{
						Q_Sampler[SamplerSelect].timer = (uint16_t)(( start + SampleSet[SamplerSelect]. sample_1 ) - now_minute );	//	ʣ������ʱ�䣨����ʱ��
					}
					else
					{
						Q_Sampler[SamplerSelect].timer = 0u;
					}
					break;
				case enumBySum:	//	���۵��磬���ۼ�ʱ������
					if ( File.sum_min         < SampleSet[SamplerSelect].sample_1 )
					{
						Q_Sampler[SamplerSelect].timer = SampleSet[SamplerSelect]. sample_1 - File.sum_min;			//	ʣ������ʱ�䣨����ʱ��
					}
					else
					{
						Q_Sampler[SamplerSelect].timer = 0u;
					}
					break;
				}

				if ( Q_Sampler[SamplerSelect].timer <= 0 ){  break; }	//	ʱ�䵽��������ʱʱ��Ϊ�㣩


				if ( SampleSwitch[SamplerSelect].Fatal )
				{
					PumpState = FALSE;
					Q_Sampler[SamplerSelect].state = state_ERROR;
				}
				else if ( SampleSwitch[SamplerSelect].Pause )
				{
					PumpState = FALSE;
					Q_Sampler[SamplerSelect].state = state_PAUSE;
				}
				else
				{
					PumpState = TRUE;











					Q_Sampler[SamplerSelect].state = state_SAMPLE;
				}
				//	Ӧ�����ظ�����
				if ( PumpState != Q_Pump[PumpSelect].xp_state )
				{
					Pump_OutCmd( PumpSelect, PumpState );
					Q_Pump[PumpSelect].xp_state = PumpState;
				}

				//	����ѹ�����ó��㣬��ֹ�������ܡ�
				if ( Configure.TSP_Pr_Portect != 0u )
				{
					FP32	Pr_Protect = Configure.TSP_Pr_Portect * 0.01f;
					
					FP32	rPr = get_Pr( PumpSelect );

					static	uint16_t	iRetry;
					
					bool	tick_en = false;
					
					if ( fabs( rPr ) < Pr_Protect )
					{
						iRetry = 0u;
					}
					else if ( iRetry < 15 )
					{
						++iRetry;
						
						tick_en = ( iRetry > 10 );
						
					}
					else
					{
						//	ȷ��ѹ��������ѹ��ִ�б�������
						SampleSwitch[SamplerSelect].Fatal = true;	//	Q_Sampler[SamplerSelect].state = state_ERROR;
					}
					
					//	delay ( 1000u );

					if ( ! tick_en )
					{
						delay ( 1000u );
					}
					else
					{	//	������ʾ
						beep();
						delay( 200u );
						beep();
						delay( 600u );
					}
				}
				else
					delay(1000u);

				tt = get_Now() / 60u;

				if ( now_minute != tt )	// 1���ӵ���
				{
					if ( state_SAMPLE == Q_Sampler[SamplerSelect].state )
					{	//	ͳ�Ʋ�������
						if ( 0u == File.sample_begin )
						{	//	���ļ�����ʼ��
							File.set_loops	= SampleSet[SamplerSelect].set_loops;
							File.run_loops  = iloop;
							File.set_time 	= SampleSet[SamplerSelect].sample_1;

							File.set_flow 	= Configure.SetFlow[SamplerSelect];

							File.sample_begin = now_minute * 60u + 1u;
						}

						if ( PumpState )
						{
							FP32	fstd = get_fstd( PumpSelect );

							FP32	Ba = get_Ba();
							FP32	Te = get_Te();// PumpSelect );
							FP32	flow = Calc_flow( fstd, Te, 0.0f, Ba );	//	Calc_flow( fstd, Tr, Pr, Ba );

							FP32	rTr = get_Tr( PumpSelect );
							FP32	rPr = get_Pr( PumpSelect );
 							if ( fabs( File.max_pr ) < fabs( rPr ))
							{
								File.max_pr = rPr;
							}
 							File.sum_tr  += rTr;
 							File.sum_pr  += rPr;
 							File.vnd     += fstd;
							File.vd      +=	flow;
 							File.sum_Ba  += Ba;
							File.sum_min += 1u;
						}















							File_Save_TSP( fname, &File );
					}
					now_minute = tt;
				}

				//	��ʾ��ѯ����
				Q_Pump[PumpSelect].sum_time = File.sum_min;
				Q_Pump[PumpSelect].vd       = File.vd;
				Q_Pump[PumpSelect].vnd      = File.vnd;

			}
			
			//	1.2 ������ǰ����
			if ( File.sum_min > 0u )
			{
				SampleSet[SamplerSelect].FileNum = fname;
				SampleSetSave();
				if ( ++fname > FileNum_Max ){  fname = 1u; }
				
				File.sample_begin = 0u;		//	����ļ�, ��־���β���ѭ������
				File_Save_TSP( fname, &File );			
				//	��¼���ۼ�����
				PumpSumTimeSave( PumpSelect, PumpSumTimeLoad( PumpSelect ) + File.sum_min );

			}
//       if( fname == 0 ){delay_us(10);beep();}     //bug

			//	�����´β���ʱ��
			switch ( Configure.Mothed_Sample )
			{
			default:
			case enumBySet:	//	���ݲ���ʱ���������У��۳����磬������ʱ������
				start = ( start + SampleSet[SamplerSelect].sample_1 );
				break;	
			case enumBySum:	//	�����ۼ�ʱ���������У����۵��磬�ӵ�ǰʱ����ʱ
				start = now_minute;
				break;							
			}
			start += SampleSet[SamplerSelect].suspend_1;
			iloop ++;
		}
			//	�����Ѿ����
			Pump_OutCmd( PumpSelect, FALSE );	Q_Pump[PumpSelect].xp_state = FALSE;
     

		//	ɾ����������
		SampleSet[SamplerSelect].start = 0u;
		SampleSet[SamplerSelect].iloop = 0u;
		SampleSetSave();
	}
	//	�������״̬
	Q_Sampler[SamplerSelect].state	= state_FINISH;
	Q_Sampler[SamplerSelect].loops	= 0u;
	Q_Sampler[SamplerSelect].timer	= 0u;

	Q_Pump[PumpSelect].vd	= 0.0f;
	Q_Pump[PumpSelect].vnd	= 0.0f;
	Q_Pump[PumpSelect].sum_time = 0u;


	//	osThreadTerminate( osThreadGetId());
}