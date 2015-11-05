void  _task_Sample_R24( void )
{
	const enum enumSamplerSelect	SamplerSelect  = Q_R24;
	const enum enumPumpSelect		PumpSelect_1 = PP_R24_A;
	const enum enumPumpSelect		PumpSelect_2 = PP_R24_B;

	BOOL	PumpState_1, PumpState_2;
	
	uint32_t	tt, now_minute = get_Now() / 60u;

	struct	uFile_R24	File;
	uint16_t	fname;	//	采样文件编号
	uint32_t	start;	//	采样开始时间
	uint8_t		iloop;	//	当前采样次数

	PumpState_1 = FALSE;
	PumpState_2 = FALSE;
	Q_Pump[PumpSelect_1].xp_state = FALSE;
	Q_Pump[PumpSelect_2].xp_state = FALSE;

	start = SampleSet[SamplerSelect].start;
	iloop = SampleSet[SamplerSelect].iloop;

	if ( 0u != start )
	{	//	初始化采样任务
		if ( 0u == iloop )
		{	//	计算第一次采样的开始时间
			switch ( Configure.Mothed_Delay )
			{
			case enumByDelay:		//	延时采样
				start = start + SampleSet[SamplerSelect].delay1;
				break;
			default:
			case enumByAccurate:	//	定时采样
				{
					uint16_t	start_hour_minute = start % 1440u;
					
					if ( start_hour_minute > SampleSet[SamplerSelect].delay1 )
					{
						start += 1440u;	//	推迟到明天的这个时间采样
					}
					start = start - start_hour_minute + SampleSet[SamplerSelect].delay1;
				}
				break;
			}
			iloop = 1u;
		}
		//	采样循环
		while (( ! SampleSwitch[SamplerSelect].Clean ) && ( iloop <= SampleSet[SamplerSelect].set_loops ))
		{
			//	保存当前采样进度
			SampleSet[SamplerSelect].start = start;
			SampleSet[SamplerSelect].iloop = iloop;
			SampleSetSave();

			Q_Sampler[SamplerSelect].loops = iloop;									//	采样状态: 计算采样次数

			//	采样执行前进行延时等待
			while (( ! SampleSwitch[SamplerSelect].Clean ) && ( now_minute < start ))
			{
				Q_Sampler[SamplerSelect].timer = (uint16_t)( start - now_minute );	//	采样状态: 剩余延时时间
				
				//	检测外部控制动作
				if ( SampleSwitch[SamplerSelect].Pause )
				{
					PumpState_1 = PumpState_2 = FALSE;
					Q_Sampler[SamplerSelect].state = state_PAUSE;					//	采样状态: 用户控制暂停
				}
				else
				{
					PumpState_1 = PumpState_2 = FALSE;
					Q_Sampler[SamplerSelect].state = state_SUSPEND;					//	采样状态: 延时(或间隔)
				}
				//	执行电机动作，并注意避免重复开关电机
				if ( PumpState_1 != Q_Pump[PumpSelect_1].xp_state )
				{
					Pump_OutCmd( PumpSelect_1, PumpState_1 );
					Q_Pump[PumpSelect_1].xp_state = PumpState_1;
				}
				if ( PumpState_2 != Q_Pump[PumpSelect_2].xp_state )
				{
					Pump_OutCmd( PumpSelect_2, PumpState_2 );
					Q_Pump[PumpSelect_2].xp_state = PumpState_2;
				}
				delay( 1000u );
				
				now_minute = get_Now() / 60u;

				//	显示查询数据
				Q_Pump[PumpSelect_1].sum_time = 0u;
				Q_Pump[PumpSelect_1].vnd      = 0.0f;
				Q_Pump[PumpSelect_2].sum_time = 0u;
				Q_Pump[PumpSelect_2].vnd      = 0.0f;
			}

			//	采样执行前，确定要保存的文件(可能需要初始化，也可能需要恢复)
			fname = SampleSet[SamplerSelect].FileNum;		//	取最后保存文件号
			if ( ++fname > FileNum_Max ){  fname = 1u; }	//	计算要使用文件号
			File_Load_R24 ( fname, &File );
			if ( 0u == File.sample_begin )
			{	//	清空统计数据，主要目的在于开始采样前显示的时候显示零
				File.sum_min [Q_PP1] = 0u;
				File.max_pr  [Q_PP1] = 0.0f;
				File.sum_tr  [Q_PP1] = 0.0f;
				File.sum_pr  [Q_PP1] = 0.0f;
				File.vnd     [Q_PP1] = 0.0f;
				File.sum_Ba           = 0.0f;
				File.sum_min [Q_PP2] = 0u;
				File.max_pr  [Q_PP2] = 0.0f;
				File.sum_tr  [Q_PP2] = 0.0f;
				File.sum_pr  [Q_PP2] = 0.0f;
				File.vnd     [Q_PP2] = 0.0f;
			}

			//	1.采样，执行采样
			while ( ! SampleSwitch[SamplerSelect].Clean ) // && 判断采样时间到
			{
				//	采样状态: 剩余采样时间
				switch ( Configure.Mothed_Sample )
				{
				default:
				case enumBySet:	//	扣除掉电，按设置时间运行
					if ( now_minute < ( start + SampleSet[SamplerSelect].sample_1 ))
					{
						Q_Sampler[SamplerSelect].timer = (uint16_t)(( start + SampleSet[SamplerSelect]. sample_1 ) - now_minute );	//	剩余运行时间（倒计时）
					}
					else
					{
						Q_Sampler[SamplerSelect].timer = 0u;
					}
					break;
				case enumBySum:	//	不扣掉电，按累计时间运行(双路,A采大于B采，按A路运行即可)
					if ( File.sum_min[Q_PP1] < SampleSet[SamplerSelect].sample_1 ) 
					{
						Q_Sampler[SamplerSelect].timer = SampleSet[SamplerSelect]. sample_1 - File.sum_min[Q_PP1];		//	剩余运行时间（倒计时）
					}
					else
					{
						Q_Sampler[SamplerSelect].timer = 0u;
					}
					break;
				}

				if ( Q_Sampler[SamplerSelect].timer <= 0 ){  break; }	//	判断采样时间到

				//	检测外部控制动作
				if ( SampleSwitch[SamplerSelect].Fatal )
				{
					PumpState_1 = PumpState_2 = FALSE;
					Q_Sampler[SamplerSelect].state = state_ERROR;					//	采样状态: 发生故障暂停
				}
				else if ( SampleSwitch[SamplerSelect].Pause )
				{
					PumpState_1 = PumpState_2 = FALSE;
					Q_Sampler[SamplerSelect].state = state_PAUSE;					//	采样状态: 用户控制暂停
				}
				else
				{
					PumpState_1 = TRUE;
					//	PumpState_2 = ?;
					switch ( Configure.Mothed_Sample )
					{
					default:
					case enumBySet:	//	扣除掉电，按设置时间运行
						PumpState_2 = ( now_minute < ( start + SampleSet[SamplerSelect].sample_2 ));
						break;
					case enumBySum:	//	不扣掉电，按累计时间运行(双路,A采大于B采，按A路运行即可)
						PumpState_2 =  ( File.sum_min[Q_PP2] < SampleSet[SamplerSelect].sample_2 );
						break;
					}
					Q_Sampler[SamplerSelect].state = state_SAMPLE;				//	采样状态: 正常采样
				}
				//	执行电机动作，并注意避免重复开关电机
				if ( PumpState_1 != Q_Pump[PumpSelect_1].xp_state )
				{
					Pump_OutCmd( PumpSelect_1, PumpState_1 );
					Q_Pump[PumpSelect_1].xp_state = PumpState_1;
				}
				if ( PumpState_2 != Q_Pump[PumpSelect_2].xp_state )
				{
					Pump_OutCmd( PumpSelect_2, PumpState_2 );
					Q_Pump[PumpSelect_2].xp_state = PumpState_2;
				}
				delay( 1000u );

				tt = get_Now() / 60u;
				if ( now_minute != tt )	// 1分钟到？
				{
					if ( state_SAMPLE == Q_Sampler[SamplerSelect].state )
					{	//	统计前初始化
						if ( 0u == File.sample_begin )
						{	//	空文件，初始化
							File.set_loops = SampleSet[SamplerSelect].set_loops;
							File.run_loops = iloop;
							File.set_time[Q_PP1] = SampleSet[SamplerSelect].sample_1;
							File.set_time[Q_PP2] = SampleSet[SamplerSelect].sample_2;
							File.set_flow[Q_PP1] = Configure.SetFlow[PumpSelect_1];
							File.set_flow[Q_PP2] = Configure.SetFlow[PumpSelect_2];
							File.sample_begin = now_minute * 60u + 1u;	//	在实际开始采样前记录开始时间
						}
						//	统计运行数据
						if ( PumpState_1 )
						{
							FP32	Ba = get_Ba();
							FP32	Tr = get_Tr( PumpSelect_1 );
							FP32	Pr = get_Pr( PumpSelect_1 );

							if ( fabs( File.max_pr[Q_PP1] ) < fabs( Pr ))
							{
								File.max_pr[Q_PP1] = Pr;
							}
							File.sum_tr  [Q_PP1] += Tr;
							File.sum_pr  [Q_PP1] += Pr;
							File.vnd     [Q_PP1] += get_fstd( PumpSelect_1 );

							File.sum_Ba  += Ba;
							File.sum_min [Q_PP1] += 1u;
						}

						if ( PumpState_2 )
						{
							FP32	Tr = get_Tr( PumpSelect_2 );
							FP32	Pr = get_Pr( PumpSelect_2 );

							if ( fabs( File.max_pr[Q_PP2] ) < fabs( Pr ))
							{
								File.max_pr[Q_PP2] = Pr;
							}
							File.sum_tr  [Q_PP2] += Tr;
							File.sum_pr  [Q_PP2] += Pr;
							File.vnd     [Q_PP2] += get_fstd( PumpSelect_2 );
							File.sum_min [Q_PP2] += 1u;
						}

						File_Save_R24 ( fname, &File );
					}
					now_minute = tt;
				}

				//	显示查询数据
				Q_Pump[PumpSelect_1].sum_time = File.sum_min [Q_PP1];
				Q_Pump[PumpSelect_1].vnd      = File.vnd     [Q_PP1];
 
				Q_Pump[PumpSelect_2].sum_time = File.sum_min [Q_PP2];
				Q_Pump[PumpSelect_2].vnd      = File.vnd     [Q_PP2];
 
			}

			//	完成一次采样，对文件进行汇总
			if (( 0u != File.sum_min[Q_PP1] ) || ( 0u != File.sum_min[Q_PP2] ))
			{	//	采样统计数据非空时执行
				SampleSet[SamplerSelect].FileNum = fname;
				SampleSetSave();
				if ( ++fname > FileNum_Max ){  fname = 1u; }
				//	记录泵累计运行
				PumpSumTimeSave( PumpSelect_1, PumpSumTimeLoad( PumpSelect_1 ) + File.sum_min[Q_PP1] );
				PumpSumTimeSave( PumpSelect_2, PumpSumTimeLoad( PumpSelect_2 ) + File.sum_min[Q_PP2] );
			}
			//	如果统计数据非空，清空的是新文件，否则，继续使用当前文件
			File.sample_begin = 0u;
			File_Save_R24 ( fname, &File );			

			//	计算下次采样的 次数、开始时间
			switch ( Configure.Mothed_Sample )
			{
			default:
			case enumBySet:	//	根据采样时间设置运行，扣除掉电，按设置时间运行
				start = ( start + SampleSet[SamplerSelect].sample_1 );
				break;	
			case enumBySum:	//	根据累计时间设置运行，不扣掉电，从当前时间延时
				start = now_minute;
				break;
			}
			start += SampleSet[SamplerSelect].suspend_1;
			iloop ++;
		}
		//	采样已经完成，强制执行一次关泵动作（忽略记录的泵状态）
			Pump_OutCmd( PumpSelect_1, FALSE );	Q_Pump[PumpSelect_1].xp_state = FALSE;
			Pump_OutCmd( PumpSelect_2, FALSE );	Q_Pump[PumpSelect_2].xp_state = FALSE;
				
	
		//	删除采样任务
		SampleSet[SamplerSelect].start = 0u;
		SampleSet[SamplerSelect].iloop = 0u;
		SampleSetSave();
	}
	//	采样全部完成
	Q_Sampler[SamplerSelect].state	= state_FINISH;
	Q_Sampler[SamplerSelect].loops	= 0u;
	Q_Sampler[SamplerSelect].timer	= 0u;

	Q_Pump[PumpSelect_1].sum_time = 0u;
	Q_Pump[PumpSelect_1].vnd	  = 0.0f;
 
	Q_Pump[PumpSelect_2].sum_time = 0u;
	Q_Pump[PumpSelect_2].vnd	  = 0.0f;
 

	//  osThreadTerminate( osThreadGetId());
}
/**/
